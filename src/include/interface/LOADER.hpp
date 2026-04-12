class _Loader {
public:
    virtual bool get_bad_news() = 0;
    virtual bool get_mem_info() = 0;
    virtual bool get_dev_info() = 0;
    virtual bool get_cpu_info() = 0;

    virtual unsigned int read_LBA(unsigned char *buffer, unsigned from_LBA, unsigned to_LBA) = 0;
    virtual unsigned int write_LBA(unsigned char *buffer, unsigned from_LBA, unsigned to_LBA) = 0;
    virtual unsigned long long open_file(unsigned char *buffer, const char *filename) = 0;
    virtual bool close_file(unsigned char *buffer, unsigned long long handle) = 0;
    virtual unsigned read_file(unsigned char *buffer, unsigned long long handle, unsigned from_byte, unsigned to_byte) = 0;
    virtual unsigned write_file(unsigned char *buffer, unsigned long long handle, unsigned from_byte, unsigned to_byte) = 0;
    
    virtual void kprint(const char *message) = 0;
    virtual char *kinput() = 0;
    
    virtual unsigned long long run(int func(const char *args)) = 0;
    virtual bool wait_for(unsigned long long handle) = 0;
    virtual bool cut_it_ (unsigned long long handle) = 0;
    virtual bool end(unsigned long long handle) = 0;
};