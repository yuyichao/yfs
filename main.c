#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

static int yfs_getattr(const char *path, struct stat *stbuf)
{
	return stat(path,stbuf);
}

static int yfs_readlink(const char *path, char *buf, size_t bufsize)
{
	return readlink(path, buf, bufsize);
}

static int yfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if(strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, yfs_path + 1, NULL, 0);

	return 0;
}

static int yfs_open(const char *path, struct fuse_file_info *fi)
{
	if(strcmp(path, yfs_path) != 0)
		return -ENOENT;

	if((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int yfs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	if(strcmp(path, yfs_path) != 0)
		return -ENOENT;

	len = strlen(yfs_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, yfs_str + offset, size);
	} else {
		size = 0;
	}
	return size;
}

static struct fuse_operations yfs_oper = {
	.getattr = yfs_getattr,
	.readdir = yfs_readdir,
	.open = yfs_open,
	.read = yfs_read,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &yfs_oper);
}
