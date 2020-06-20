#ifdef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#define PageSize 4096  						//4K

#include <string>

class Page{
public:
	Page();
	~Page(){};
	void initialize();
	inline void setFileName(std::string name);
	inline std::string getFileName();
	inline void setBlockId(int id);
	inline int getBlockId();
	inline void setPinCount(int cnt);
	inline int getPinCount();
	void setDirty(bool dirty);
    bool getDirty();
    void setRef(bool ref);
    bool getRef();
    void setAvailable(bool available);
    bool getAvailable();
    char* getBuffer();
private:
	char buffer[PageSize];
	std::string file_name;
	int block_id;
	int pin_count;
	bool dirty;
	bool ref;
	bool available;
};

class BufferManager{
public:
	BufferManager();
    BufferManager(int size);
    // ��������
    ~BufferManager();
    // ͨ��ҳ�ŵõ�ҳ�ľ��(һ��ҳ��ͷ��ַ)
    char* getPage(std::string file_name , int block_id);
    // ���page_id����Ӧ��ҳ�Ѿ����޸�
    void modifyPage(int page_id);
    // ��סһ��ҳ
    void pinPage(int page_id);
    // ���һ��ҳ�Ķ�ס״̬(��Ҫע�����һ��ҳ���ܱ���ζ�ס���ú���ֻ�ܽ��һ��)
    // �����Ӧҳ��pin_count_Ϊ0���򷵻�-1
    int unpinPage(int page_id);
    // ����Ӧ�ڴ�ҳд���Ӧ�ļ��Ķ�Ӧ�顣����ķ���ֵΪint�����о���ʵûʲô�ã�������Ϊvoid
    int flushPage(int page_id , std::string file_name , int block_id);
    // ��ȡ��Ӧ�ļ��Ķ�Ӧ�����ڴ��е�ҳ�ţ�û���ҵ�����-1
    int getPageId(std::string file_name , int block_id);
private:
	Page* ptr_page;
	int size;	
	int current_pos;//�����ĵ�ǰpage 
    // ��ȡһ�����õ�ҳ��ҳ��(�ڲ���װ��ʱ���滻���ԣ���ʹ���߲���Ҫ֪����Щ)
    int getEmptyPageId();
    // ����Ӧ�ļ��Ķ�Ӧ�������Ӧ�ڴ�ҳ�������ļ������ڷ���-1�����򷵻�0
    int loadDiskBlock(int page_id , std::string file_name , int block_id);
};

inline void Page::setFileName(std::string name)
{
	file_name = name; 
} 

inline std::string Page::getFileName()
{
	return file_name;
}

inline void Page::setBlockId(int id)
{
	block_id = id; 
} 

inline int Page::getBlockId()
{
	return block_id;
}

inline void Page::setPinCount(int cnt)
{
	pin_count = cnt;
}

inline int Page::getPinCount()
{
	return pin_count;
}

inline void Page::setDirty(bool dirty) 
{
    this->dirty = dirty;
}

inline bool Page::getDirty() 
{
    return dirty;
}

inline void Page::setRef(bool ref) 
{
    this->ref = ref;
}

inline bool Page::getRef() 
{
    return ref;
}

inline void Page::setAvailable(bool available) 
{
    this->available = available;
}

inline bool Page::getAvailable() 
{
    return available;
}

inline char* Page::getBuffer() 
{
    return buffer;
}


#endif
