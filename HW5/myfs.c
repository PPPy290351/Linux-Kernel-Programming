#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagemap.h>      /* PAGE_CACHE_SIZE */
#include <linux/fs.h>           /* This is where libfs stuff is declared */
#include <asm/atomic.h>
#include <asm/uaccess.h>        /* copy_to_user */
 
 
MODULE_LICENSE("GPL");
 
#define MYFS_MAGIC 0x20210620
 
static atomic_t a, b, add, sub;
 
static struct inode *myfs_make_inode(struct super_block *sb, int mode)
{
        struct inode *ret = new_inode(sb);
 
        if (ret) {
                ret->i_mode = mode;
                ret->i_uid = ret->i_gid = 0;
                ret->i_blocks = 0;
                ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
        }
        return ret;
}
 
static int myfs_open(struct inode *inode, struct file *filp)
{
        filp->private_data = inode->i_private;
        return 0;
}
 
#define TMPSIZE 20
 
static ssize_t myfs_read_file(struct file *filp, char *buf,
                size_t count, loff_t *offset)
{
        atomic_t *data_value = (atomic_t *) filp->private_data;
        int v, len;
        char tmp[TMPSIZE];
 
        v = atomic_read(data_value);
        if (*offset > 0)
                v -= 1;  /* the value returned when offset was zero */
        if(v == -1){
                v = atomic_read(&a) + atomic_read(&b);
        }else if(v == -2){
                v = atomic_read(&a) - atomic_read(&b);
        }
        len = snprintf(tmp, TMPSIZE, "%d\n", v);
        if (*offset > len)
                return 0;
        if (count > len - *offset)
                count = len - *offset;
 
        if (copy_to_user(buf, tmp + *offset, count))
                return -EFAULT;
        *offset += count;
        return count;
}
 
static ssize_t myfs_write_file(struct file *filp, const char *buf,
                size_t count, loff_t *offset)
{
        atomic_t *counter = (atomic_t *) filp->private_data;
        char tmp[TMPSIZE];
 
        if (*offset != 0)
                return -EINVAL;
 
        if (count >= TMPSIZE)
                return -EINVAL;
        memset(tmp, 0, TMPSIZE);
        if (copy_from_user(tmp, buf, count))
                return -EFAULT;
 
        atomic_set(counter, simple_strtol(tmp, NULL, 10));
        return count;
}
 
 
static struct file_operations myfs_file_ops = {
        .open   = myfs_open,
        .read   = myfs_read_file,
        .write  = myfs_write_file,
};
 
 
static struct dentry *myfs_create_file (struct super_block *sb,
                struct dentry *dir, const char *name,
                atomic_t *counter)
{
        struct dentry *dentry;
        struct inode *inode;
        struct qstr qname;
 
        qname.name = name;
        qname.len = strlen (name);
        qname.hash = full_name_hash(name, qname.len);
 
        dentry = d_alloc(dir, &qname);
        if (! dentry)
                goto out;
        inode = myfs_make_inode(sb, S_IFREG | 0644);
        if (! inode)
                goto out_dput;
        inode->i_fop = &myfs_file_ops;
        inode->i_private = counter;
 
        d_add(dentry, inode);
        return dentry;
 
  out_dput:
        dput(dentry);
  out:
        return 0;
}
 
 
static struct dentry *myfs_create_dir (struct super_block *sb,
                struct dentry *parent, const char *name)
{
        struct dentry *dentry;
        struct inode *inode;
        struct qstr qname;
 
        qname.name = name;
        qname.len = strlen (name);
        qname.hash = full_name_hash(name, qname.len);
        dentry = d_alloc(parent, &qname);
        if (! dentry)
                goto out;
 
        inode = myfs_make_inode(sb, S_IFDIR | 0644);
        if (! inode)
                goto out_dput;
        inode->i_op = &simple_dir_inode_operations;
        inode->i_fop = &simple_dir_operations;
 
        d_add(dentry, inode);
        return dentry;
 
  out_dput:
        dput(dentry);
  out:
        return 0;
}
 
 
static void myfs_create_files (struct super_block *sb, struct dentry *root)
{
        struct dentry *input;
        struct dentry *output;
 
        atomic_set(&a, 0);
        atomic_set(&b, 0);

        input = myfs_create_dir(sb, root, "input");
        output = myfs_create_dir(sb, root, "output");

        if(input){
                myfs_create_file(sb, input, "a", &a);
                myfs_create_file(sb, input, "b", &b);
        }else{
                printk(KERN_ALERT "Error while initializing input\n");
        }

        atomic_set(&add, -1);
        atomic_set(&sub, -2);
 
        if(output){
                myfs_create_file(sb, output, "add", &add);
                myfs_create_file(sb, output, "sub", &sub);
        }else{
                printk(KERN_ALERT "Error while initializing output\n");
        }
}
 
 
 
static struct super_operations myfs_s_ops = {
        .statfs         = simple_statfs,
        .drop_inode     = generic_delete_inode,
};
 
static int myfs_fill_super (struct super_block *sb, void *data, int silent)
{
        struct inode *root;
        struct dentry *root_dentry;
 
        sb->s_blocksize = PAGE_CACHE_SIZE;
        sb->s_blocksize_bits = PAGE_CACHE_SHIFT;
        sb->s_magic = MYFS_MAGIC;
        sb->s_op = &myfs_s_ops;
 
        root = myfs_make_inode (sb, S_IFDIR | 0755);
        if (! root)
                goto out;
        root->i_op = &simple_dir_inode_operations;
        root->i_fop = &simple_dir_operations;
 
        root_dentry = d_alloc_root(root);
        if (! root_dentry)
                goto out_iput;
        sb->s_root = root_dentry;
 
        myfs_create_files (sb, root_dentry);
        return 0;
 
  out_iput:
        iput(root);
  out:
        return -ENOMEM;
}
 
static struct dentry *myfs_get_super(struct file_system_type *fst,
                int flags, const char *devname, void *data)
{
        return mount_bdev(fst, flags, devname, data, myfs_fill_super);
}
 
static struct file_system_type myfs_type = {
        .owner          = THIS_MODULE,
        .name           = "myfs",
        .mount          = myfs_get_super,
        .kill_sb        = kill_litter_super,
};
 
static int __init myfs_init(void)
{
        return register_filesystem(&myfs_type);
}
 
static void __exit myfs_exit(void)
{
        unregister_filesystem(&myfs_type);
}
 
module_init(myfs_init);
module_exit(myfs_exit);