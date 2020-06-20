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
    // 析构函数
    ~BufferManager();
    // 通过页号得到页的句柄(一个页的头地址)
    char* getPage(std::string file_name , int block_id);
    // 标记page_id所对应的页已经被修改
    void modifyPage(int page_id);
    // 钉住一个页
    void pinPage(int page_id);
    // 解除一个页的钉住状态(需要注意的是一个页可能被多次钉住，该函数只能解除一次)
    // 如果对应页的pin_count_为0，则返回-1
    int unpinPage(int page_id);
    // 将对应内存页写入对应文件的对应块。这里的返回值为int，但感觉其实没什么用，可以设为void
    int flushPage(int page_id , std::string file_name , int block_id);
    // 获取对应文件的对应块在内存中的页号，没有找到返回-1
    int getPageId(std::string file_name , int block_id);
private:
	Page* ptr_page;
	int size;	
	int current_pos;//遍历的当前page 
    // 获取一个闲置的页的页号(内部封装了时钟替换策略，但使用者不需要知道这些)
    int getEmptyPageId();
    // 讲对应文件的对应块载入对应内存页，对于文件不存在返回-1，否则返回0
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
