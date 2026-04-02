#include "mmap/mmap_aux.h"

#include <cstring>
#include "utils/file_util.h"
#include "utils/sys_util.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "defer.h"
namespace logger {

    size_t GetPageSize()
    {
        return sysconf(_SC_PAGESIZE);
    }

    static constexpr size_t kDefalutCapacity = 512 * 1024; // 512KB

    MMapAux::MMapAux(fpath file_path) : file_path_(std::move(file_path)), handle_(nullptr), capacity_(0) {
        size_t file_size = fs::GetFileSize(file_path_);
        size_t dst_size = std::max(file_size, kDefalutCapacity);
        Reserve_(dst_size); // 分配内存映射
        Init_();    // 完成内存映射的初始化和相关状态设置
    }

    // 完成内存映射的初始化和相关状态设置
    void MMapAux::Init_() {
        MmapHeader* header = Header_(); // 获取内存映射头指针
        if (!header) {
            return;
        }
        if (header->magic != MmapHeader::kMagic) {
            header->magic = MmapHeader::kMagic; // 验证内存映射头是否有效
            header->size = 0;
        }
    }

    void MMapAux::Resize(size_t new_size) {
        if (!IsValid_()) {
            return;
        }
        EnsureCapacity_(new_size); // 确保内存映射有足够的容量
        Header_()->size = new_size; // 更新内存映射头的大小
    }

    void MMapAux::Clear() {
        if (!IsValid_()) {
            return;
        }
        Header_()->size = 0; // 重置内存映射头的大小
    }

    uint8_t* MMapAux::Data() const {
        if (!IsValid_()) {
            return nullptr;
        }
        return static_cast<uint8_t*>(handle_) + sizeof(MmapHeader); // 返回内存映射数据的指针
    }

    double MMapAux::GetRatio() const {
        if (!IsValid_()) {
            return 0.0;
        }
        return static_cast<double>(Size()) / (Capacity_() - sizeof(MmapHeader)); // 返回内存映射使用比例
    }

    void MMapAux::Push(const void* data, size_t size) {
        if (!IsValid_()) {
            return;
        }
        size_t new_size = Size() + size;
        EnsureCapacity_(new_size); // 确保内存映射有足够的容量
        memcpy(Data() + Size(), data, size); // 复制数据到内存映射
        Header_()->size = new_size; // 更新内存映射头的大小
    }

    size_t MMapAux::Size() const {
        if (!IsValid_()) {
            return 0;
        }
        return Header_()->size; // 返回内存映射头的大小
    }

    MMapAux::MmapHeader *MMapAux::Header_() const
    {
        if (!handle_)
        {
            return nullptr;
        }
        if (capacity_ < sizeof(MmapHeader))
        {
            return nullptr;
        }
        return static_cast<MmapHeader *>(handle_);
    }

    bool MMapAux::IsValid_() const {
        MmapHeader* header = Header_();
        if (!header) {
            return false;
        }
        return header->magic == MmapHeader::kMagic; // 验证内存映射头是否有效
    }

    static size_t GetValidCapacity_(size_t size) {
        size_t page_size = GetPageSize();
        return (size + page_size - 1) / page_size * page_size;
    }

    void MMapAux::Reserve_(size_t new_size) {
        if (new_size <= capacity_) {
            return;
        }
        new_size = GetValidCapacity_(new_size);
        if (new_size == capacity_) {
            return;
        }
        Unmap_();
        TryMap_(new_size);
        capacity_ = new_size;
    }

    void MMapAux::EnsureCapacity_(size_t new_size)
    {
        size_t real_size = new_size + sizeof(MmapHeader);
        if (real_size <= capacity_)
        {
            return;
        }
        auto dst_capacity = capacity_;
        while (dst_capacity < real_size)
        {
            dst_capacity += GetPageSize();
        }
        Reserve_(dst_capacity);
    }

    bool MMapAux::TryMap_(size_t capacity)
    {
        int fd = open(file_path_.string().c_str(), O_RDWR | O_CREAT, S_IRWXU);
        LOG_DEFER
        {
            if (fd != -1)
            {
                close(fd);
            }
        };

        if (fd == -1)
        {
            return false;
        }
        else
        {
            ftruncate(fd, capacity);
        }

        handle_ = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        return handle_ != MAP_FAILED;
    }

    void MMapAux::Unmap_()
    {
        if (handle_)
        {
            munmap(handle_, capacity_);
        }
        handle_ = NULL;
    }

    void MMapAux::Sync_()
    {
        if (handle_)
        {
            msync(handle_, capacity_, MS_SYNC);
        }
    }
}