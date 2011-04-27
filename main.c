#define FUSE_USE_VERSION 26
#include <unistd.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

FILE *logfp;
int count = 0;

static int yfs_create(const char* path, mode_t mode
		      , struct fuse_file_info *fi)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s, mode= %o\n", path, mode);
	fflush(logfp);
	int fd = creat(path, mode);
	if (fd == -1)
		return -errno;
	fi->fh = fd;
	return 0;
}

static int yfs_access(const char *path, int amode)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s, amode= %o\n", path, amode);
	fflush(logfp);
	return access(path, amode) == -1 ? -errno : 0;
}

static int yfs_getattr(const char *path, struct stat *stbuf)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return lstat(path,stbuf) == -1 ? -errno : 0;
}

static int yfs_readlink(const char *path, char *buf, size_t bufsize)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s, bufsize= %d\n", path, bufsize);
	fflush(logfp);
	int size = readlink(path, buf, bufsize);
	if (size == -1)
		return -errno;
	if (size < bufsize) {
		buf[size] = '\0';
		return 0;
	}
	return -ENOBUFS;
}

static int yfs_mknod(const char *path, mode_t mode, dev_t dev)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s, mode= %o\n", path, mode);
	fflush(logfp);
	return mknod(path, mode, dev) == -1 ? -errno : 0;
}

static int yfs_mkdir(const char *path, mode_t mode)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s, mode= %o\n", path, mode);
	fflush(logfp);
	return mkdir(path, mode) == -1 ? -errno : 0;
}

static int yfs_unlink(const char *path)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return unlink(path) == -1 ? -errno : 0;
}

static int yfs_rmdir(const char *path)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return rmdir(path) == -1 ? -errno : 0;
}

static int yfs_symlink(const char *path1, const char *path2)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path1= %s, path2= %s\n", path1, path2);
	fflush(logfp);
	return symlink(path1, path2) == -1 ? -errno : 0;
}

static int yfs_rename(const char *old, const char *new)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "old= %s, new= %s\n", old, new);
	fflush(logfp);
	return rename(old, new) == -1 ? -errno : 0;
}

static int yfs_link(const char *path1, const char *path2)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path1= %s, path2= %s\n", path1, path2);
	fflush(logfp);
	return link(path1, path2) == -1 ? -errno : 0;
}

static int yfs_chmod(const char *path, mode_t mode)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s, mode= %o\n", path, mode);
	fflush(logfp);
	return chmod(path, mode) == -1 ? -errno : 0;
}

static int yfs_chown(const char *path, uid_t owner, gid_t group)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s, owner= %d, group= %d\n", path, owner, group);
	fflush(logfp);
	return chown(path, owner, group) == -1 ? -errno : 0;
}

static int yfs_truncate(const char *path, off_t length)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return truncate(path, length) == -1 ? -errno : 0;
}

static int yfs_utime(const char *path, struct utimbuf *times)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return utime(path, times) == -1 ? -errno : 0;
}

static int yfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	struct dirent *dp;
	DIR *dirp = (DIR *)fi->fh;

	while (dp = readdir(dirp)) {
		if (filler(buf, dp->d_name, NULL, 0))
			return -ENOBUFS;
	}
	return 0;
}

static int yfs_open(const char *path, struct fuse_file_info *fi)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	int fd = open(path, fi->flags);
	if (fd < 0)
		return -errno;
	fi->fh = fd;
	return 0;
}

static int yfs_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd = (int)fi->fh;
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	int count = pread(fd, buf, size, offset);
	if (count == -1)
		return -errno;
	return count;
}

static int yfs_write(const char *path, const char *buf, size_t size, off_t offset,
		     struct fuse_file_info *fi)
{
	int fd = (int)fi->fh;
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	int count = pwrite(fd, buf, size, offset);
	if (count == -1)
		return -errno;
	return count;
}

static int yfs_statfs(const char *path, struct statvfs *buf)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return statvfs(path, buf) == -1 ? -errno : 0;
}

static int yfs_release(const char *path, struct fuse_file_info *fi)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return close((int)fi->fh) == -1 ? -errno : 0;
}

static int yfs_opendir(const char *path, struct fuse_file_info *fi)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	DIR *dirp = opendir(path);
	if (!dirp)
		return -errno;
	fi->fh = (uint64_t)dirp;
	return 0;
}

static int yfs_releasedir(const char *path, struct fuse_file_info *fi)
{
	fprintf(logfp, "%d@%d:in %s()\n", count++, time(NULL), __func__);
	fprintf(logfp, "path= %s\n", path);
	fflush(logfp);
	return closedir((DIR *)fi->fh) == -1 ? -errno : 0;
}

static struct fuse_operations yfs_oper = {
	.getattr = yfs_getattr,
	.create = yfs_create,
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
	.access = yfs_access,
	.statfs = yfs_statfs,
	.release = yfs_release,
	.opendir = yfs_opendir,
	.releasedir = yfs_releasedir,
};

int main(int argc, char *argv[])
{
	int tmpfd = dup(STDOUT_FILENO);
	if (!(logfp = fdopen(tmpfd, "a"))){
		printf("error:open\n");
		return -1;
	}
	return fuse_main(argc, argv, &yfs_oper, NULL);
}
