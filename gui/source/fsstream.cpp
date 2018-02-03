/*  This file is part of Checkpoint
>	Copyright (C) 2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

#include "fsstream.h"

#define BUFFER_SIZE 0x100000

FSStream::FSStream(FS_Archive archive, std::u16string path, u32 flags)
{
	loaded = false;
	size = 0;
	offset = 0;
	
	res = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_UTF16, path.data()), flags, 0);
	if (R_SUCCEEDED(res))
	{
		FSFILE_GetSize(handle, (u64*)&size);
		loaded = true;		
	}
}

FSStream::FSStream(FS_Archive archive, std::u16string path, u32 flags, u32 _size)
{
	loaded = false;
	size = _size;
	offset = 0;
	
	res = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_UTF16, path.data()), flags, 0);
	if (R_FAILED(res))
	{
		res = FSUSER_CreateFile(archive, fsMakePath(PATH_UTF16, path.data()), flags, size);
		if (R_SUCCEEDED(res))
		{
			res = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_UTF16, path.data()), flags, 0);
			if (R_SUCCEEDED(res))
			{
				loaded = true;
			}
		}
	}	
}

Result FSStream::close(void)
{
	res = FSFILE_Close(handle);
	return res;
}

bool FSStream::getLoaded(void)
{
	return loaded;
}

Result FSStream::getResult(void)
{
	return res;
}

u32 FSStream::getSize(void)
{
	return size;
}

u32 FSStream::read(void *buf, u32 sz)
{
	u32 rd = 0;
	res = FSFILE_Read(handle, &rd, offset, buf, sz);
	offset += rd;
	return rd;
}

u32 FSStream::write(void *buf, u32 sz)
{
	u32 wt = 0;
	res = FSFILE_Write(handle, &wt, offset, buf, sz, FS_WRITE_FLUSH);
	offset += wt;
	return wt;
}

bool FSStream::isEndOfFile(void)
{
	return offset > size;
}

bool fileExist(FS_Archive archive, std::u16string path)
{
	FSStream stream(archive, path, FS_OPEN_READ);
	bool exist = stream.getLoaded();
	stream.close();
	return exist;
}

void copyFile(FS_Archive srcArch, FS_Archive dstArch, std::u16string srcPath, std::u16string dstPath)
{
	u32 size = 0;
	FSStream input(srcArch, srcPath, FS_OPEN_READ);
	if (input.getLoaded())
	{
		size = input.getSize() > BUFFER_SIZE ? BUFFER_SIZE : input.getSize();
	}
	else
	{
		return;
	}
	
	FSStream output(dstArch, dstPath, FS_OPEN_WRITE, input.getSize());
	if (output.getLoaded())
	{
		u8* buf = new u8[size];
		do {
			u32 rd = input.read(buf, size);
			output.write(buf, rd);
		} while(input.isEndOfFile());

		delete[] buf;		
	}

	input.close();
	output.close();
}

/*Result copyDirectory(FS_Archive srcArch, FS_Archive dstArch, std::u16string srcPath, std::u16string dstPath)
{
	Result res = 0;
	bool quit = false;
	Directory items(srcArch, srcPath);
	
	if (!items.getLoaded())
	{
		return items.getError();
	}
	
	for (size_t i = 0, sz = items.getCount(); i < sz && !quit; i++)
	{
		std::u16string newsrc = srcPath + items.getItem(i);
		std::u16string newdst = dstPath + items.getItem(i);
		
		if (items.isFolder(i))
		{
			res = createDirectory(dstArch, newdst);
			
			if (R_FAILED(res))
			{
				quit = true;
			}
			else
			{
				newsrc += u8tou16("/");
				newdst += u8tou16("/");
				res = copyDirectory(srcArch, dstArch, newsrc, newdst);
			}
		}
		else
		{
			drawCopy(items.getItem(i));
			copyFile(srcArch, dstArch, newsrc, newdst);
		}
	}
	
	return res;
}*/

Result createDirectory(FS_Archive archive, std::u16string path)
{
	return FSUSER_CreateDirectory(archive, fsMakePath(PATH_UTF16, path.data()), 0);
}

bool directoryExist(FS_Archive archive, std::u16string path)
{
	Handle handle;

	if (R_FAILED(FSUSER_OpenDirectory(&handle, archive, fsMakePath(PATH_UTF16, path.data()))))
	{
		return false;
	}

	if (R_FAILED(FSDIR_Close(handle)))
	{
		return false;
	}

	return true;
}
