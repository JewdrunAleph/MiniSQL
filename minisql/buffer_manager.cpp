#include "buffer_manager.h"

Page::Page()
{
	initialize();
}

void Page::initialize()
{
	file_name = "";
	block_id = -1;
	pin_count = 0;
	dirty = 0;
	ref = 0;
	available = 1;
	for(int i=0;i<PageSize;i++){
		buffer[i] = '\0';
	}
}

BufferManager::BufferManager(int size)
{
	ptr_page = new Page[size];
	this->size = size;
}

BufferManager::BufferManager()
{
	ptr_page = new Page[128];
	this->size = size;
}

BufferManager::~BufferManager()
{
	for(int i=0;i<size;i++){
		std::string tmp_name = ptr_page[i].getFileName();
		int tmp_id = ptr_page[i].getBlockId();
		flushPage(i, tmp_name, tmp_id);
	}
}

char* BufferManager::getPage(std::string file_name , int block_id) {
    int page_id = getPageId(file_name , block_id);
    if (page_id == -1) {
        page_id = getEmptyPageId();
        loadDiskBlock(page_id , file_name , block_id);
    }
    ptr_page[page_id].setRef(true);
    return ptr_page[page_id].getBuffer();
}

// 简单遍历获取页号
int BufferManager::getPageId(std::string file_name , int block_id) {
    for (int i = 0;i < size;i++) {
        std::string tmp_name = ptr_page[i].getFileName();
        int tmp_id = ptr_page[i].getBlockId();
        if (tmp_name == file_name && tmp_id == block_id)
            return i;
    }
    return -1;
}

// 寻找一个闲置的页
int BufferManager::getEmptyPageId() {
    // 先简单的遍历一遍，如果有闲置的直接返回其页号
    for (int i = 0;i < size;i++) {
        if (ptr_page[i].getAvailable() == true)
            return i;
    }
    // 如果所有页都已经被使用，那么需要找到一个页，将其删除掉。
    // 这里需要使用一些策略来选择哪一个页应该被删除。
    // 本程序中采用时钟替换策略。
    while (1) {
        // 如果页的ref为true，将其设为false
        if (ptr_page[current_pos].getRef() == true) {
            ptr_page[current_pos].setRef(false);
        }
        // 否则，如果页对应的pin_count为0，即页没有被锁住，那么这一页就
        // 会被删除。
        else if (ptr_page[current_pos].getPinCount() == 0) {
            // 如果这一页被改动过，需要将其写回磁盘，然后删除
            if (ptr_page[current_pos].getDirty() == true) {
                std::string file_name = ptr_page[current_pos].getFileName();
                int block_id = ptr_page[current_pos].getBlockId();
                flushPage(current_pos, file_name , block_id);
            }
            // 删除页
            ptr_page[current_pos].initialize();
            // 返回页号
            return current_pos;
        }
        // 时钟指针顺时针转动
        current_pos = (current_pos + 1) % size;
    }
}

// 核心函数之一。内存和磁盘交互的接口。
int BufferManager::flushPage(int page_id , std::string file_name , int block_id) 
{
    // 打开文件
    FILE* fp = fopen(file_name.c_str() , "r+");
    // 其实这里有写多余，因为打开一个文件读总是能成功。
    if (fp == NULL)
        return -1; 
    // 将文件指针定位到对应位置
    fseek(fp , PageSize * block_id , SEEK_SET);
    // 获取页的句柄
    char* buffer = ptr_page[page_id].getBuffer();
    // 将内存页的内容写入磁盘块
    fwrite(buffer , PageSize , 1 , fp);
    // 关闭文件
    fclose(fp);
    return 0;
}

int BufferManager::loadDiskBlock(int page_id , std::string file_name , int block_id)
{
	char* buffer = ptr_page[page_id].getBuffer();
	//初始化 
	ptr_page[page_id].initialize();
	// 打开文件
    FILE* fp = fopen(file_name.c_str() , "r");
    if (fp == NULL)
        return -1;
    fseek(fp , PageSize * block_id , SEEK_SET);
    fread(buffer , PageSize , 1 , fp);
    fclose(fp);
    ptr_page[page_id].setDirty(false);
    ptr_page[page_id].setFileName(file_name);
    ptr_page[page_id].setBlockId(block_id);
    ptr_page[page_id].setPinCount(1);
    ptr_page[page_id].setRef(true);
    ptr_page[page_id].setAvailable(false);
    return 0;
}

int BufferManager::unpinPage(int page_id)
{
	int cnt = ptr_page[page_id].getPinCount();
	if (cnt <= 0)
		return -1; 
	ptr_page[page_id].setPinCount(cnt-1);
	return 0;
}

void BufferManager::pinPage(int page_id)
{
	int cnt = ptr_page[page_id].getPinCount();
	ptr_page[page_id].setPinCount(cnt+1);
}

void BufferManager::modifyPage(int page_id) {
    ptr_page[page_id].setDirty(true);
}


