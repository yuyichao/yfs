#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

static int yfs_getattr(const char *path, struct stat *stbuf)
{
	return stat(path,stbuf);
}

static int yfs_readlink(const char *path, char *buf, size_t bufsize)
{
	return readlink(path, buf, bufsize);
}

static int yfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	return mknod(path, mode, dev);
}

static int yfs_mkdir(const char *path, mode_t mode)
{
	return mkdir(path, mode);
}

static int yfs_unlink(const char *path)
{
	return unlink(path);
}

static int yfs_rmdir(const char *path)
{
	return rmdir(path);
}

static int yfs_symlink(const char *path1, const char *path2)
{
	return symlink(path1, path2);
}

static int yfs_rename(const char *old, const char *new)
{
	return rename(old, new);
}

static int yfs_link(const char *path1, const char *path2)
{
	return link(path1, path2);
}

static int yfs_chmod(const char *path, mode_t mode)
{
	return chmod(path, mode);
}

static int yfs_chown(const char *path, uid_t owner, gid_t group)
{
	return chown(path, owner, group);
}

static int yfs_truncate(const char *path, off_t length)
{
	return truncate(path, length);
}

static int yfs_utime(const char *path, struct utimbuf *times)
{
	return utime(path, times);
}



static int yfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
	struct dirent *dp;
	DIR *dirp = (DIR *)fi->fh;

	while (dp = readdir(dirp)) {
		if (filler(buf, dp->d_name, NULL, 0))
			return -1;
	}
	return 0;
}

static int yfs_open(const char *path, struct fuse_file_info *fi)
{
	int fd = open(path, fi->flags);
	if (fd < 0)
		return errno;
	fi->fh = fd;
	return 0;
}

static int yfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd = (int)fi->fh;
	return pread(fd, buf, size, offset);
}

static int yfs_write(const char *path, const char *buf, size_t size, off_t offset,
		     struct fuse_file_info *fi)
{
	int fd = (int)fi->fh;
	return pwrite(fd, buf, size, offset);
}

static int yfs_statfs(const char *path, struct statvfs *buf)
{
	return statvfs(path, buf);
}

static int yfs_release(const char *path, struct fuse_file_info *fi)
{
	return close((int)fi->fh);
}

static int yfs_opendir(const char *path, struct fuse_file_info *fi)
{
	DIR *dirp = opendir(path);
	if (!dirp)
		return errno;
	fi->fh = (uint64_t)dirp;
	return 0;
}

static int yfs_releasedir(const char *path, struct fuse_file_info *fi)
{
	return closedir((DIR *)fi->fh);
}

static struct fuse_operations yfs_oper = {
	.getattr = yfs_getattr,
	.readlink = yfs_readlink,
	.mknod = yfs_mknod,
	.mkdir = yfs_mkdir,
	.unlink = yfs_unlink,
	.rmdir = yfs_rmdir,
	.symlink = yfs_symlink,
	.rename = yfs_rename,
	.link = yfs_link,
	.chmod = yfs_chmod,
	.chown = yfs_chown,
	.truncate = yfs_truncate,
	.utime = yfs_utime,
	.readdir = yfs_readdir,
	.open = yfs_open,
	.read = yfs_read,
	.write = yfs_write,
	.statfs = yfs_statfs,
	.release = yfs_release,
	.opendir = yfs_opendir,
	.releasedir = yfs_releasedir,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &yfs_oper, NULL);
}
