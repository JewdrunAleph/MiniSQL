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

// �򵥱�����ȡҳ��
int BufferManager::getPageId(std::string file_name , int block_id) {
    for (int i = 0;i < size;i++) {
        std::string tmp_name = ptr_page[i].getFileName();
        int tmp_id = ptr_page[i].getBlockId();
        if (tmp_name == file_name && tmp_id == block_id)
            return i;
    }
    return -1;
}

// Ѱ��һ�����õ�ҳ
int BufferManager::getEmptyPageId() {
    // �ȼ򵥵ı���һ�飬��������õ�ֱ�ӷ�����ҳ��
    for (int i = 0;i < size;i++) {
        if (ptr_page[i].getAvailable() == true)
            return i;
    }
    // �������ҳ���Ѿ���ʹ�ã���ô��Ҫ�ҵ�һ��ҳ������ɾ������
    // ������Ҫʹ��һЩ������ѡ����һ��ҳӦ�ñ�ɾ����
    // �������в���ʱ���滻���ԡ�
    while (1) {
        // ���ҳ��refΪtrue��������Ϊfalse
        if (ptr_page[current_pos].getRef() == true) {
            ptr_page[current_pos].setRef(false);
        }
        // �������ҳ��Ӧ��pin_countΪ0����ҳû�б���ס����ô��һҳ��
        // �ᱻɾ����
        else if (ptr_page[current_pos].getPinCount() == 0) {
            // �����һҳ���Ķ�������Ҫ����д�ش��̣�Ȼ��ɾ��
            if (ptr_page[current_pos].getDirty() == true) {
                std::string file_name = ptr_page[current_pos].getFileName();
                int block_id = ptr_page[current_pos].getBlockId();
                flushPage(current_pos, file_name , block_id);
            }
            // ɾ��ҳ
            ptr_page[current_pos].initialize();
            // ����ҳ��
            return current_pos;
        }
        // ʱ��ָ��˳ʱ��ת��
        current_pos = (current_pos + 1) % size;
    }
}

// ���ĺ���֮һ���ڴ�ʹ��̽����Ľӿڡ�
int BufferManager::flushPage(int page_id , std::string file_name , int block_id) 
{
    // ���ļ�
    FILE* fp = fopen(file_name.c_str() , "r+");
    // ��ʵ������д���࣬��Ϊ��һ���ļ��������ܳɹ���
    if (fp == NULL)
        return -1; 
    // ���ļ�ָ�붨λ����Ӧλ��
    fseek(fp , PageSize * block_id , SEEK_SET);
    // ��ȡҳ�ľ��
    char* buffer = ptr_page[page_id].getBuffer();
    // ���ڴ�ҳ������д����̿�
    fwrite(buffer , PageSize , 1 , fp);
    // �ر��ļ�
    fclose(fp);
    return 0;
}

int BufferManager::loadDiskBlock(int page_id , std::string file_name , int block_id)
{
	char* buffer = ptr_page[page_id].getBuffer();
	//��ʼ�� 
	ptr_page[page_id].initialize();
	// ���ļ�
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


