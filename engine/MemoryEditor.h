

class MemoryEditor
{
public:

    bool    Open;
    bool    AllowEdits;
    int     Rows;
    int     DataEditingAddr;
    bool    DataEditingTakeFocus;
    char    DataInput[32];
    char    AddrInput[32];

    MemoryEditor();

    void Draw(const char* title, unsigned char* mem_data, int mem_size, size_t base_display_addr = 0);
};