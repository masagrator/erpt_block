#include <switch.h>

int main(int argc, char* argv[]) {
    s64 size = 0;
    char file_path[FS_MAX_PATH];
    FsFileSystem* sdmc_system = fsdevGetDeviceFileSystem("sdmc");
    fsdevTranslatePath("sdmc:/atmosphere/erpt_reports", &sdmc_system, file_path);
    if (R_SUCCEEDED(fsFsGetTotalSpace(sdmc_system, file_path, &size))) {
        return 0;
    }
    while(true) {
        fsdevDeleteDirectoryRecursively("sdmc:/atmosphere/erpt_reports");
        Result rc = fsdevCreateFile("sdmc:/atmosphere/erpt_reports", 0, 1);
        if (R_FAILED(rc))
            continue;
        else break;
    }

    // note: sysmod exits here.
    // to keep it running, add a for (;;) loop (remember to sleep!)
    return 0;
}

// libnx stuff goes below
extern "C" {

// Sysmodules should not use applet*.
u32 __nx_applet_type = AppletType_None;

// Sysmodules will normally only want to use one FS session.
u32 __nx_fs_num_sessions = 1;

// Newlib heap configuration function (makes malloc/free work).
void __libnx_initheap(void) {
    static char inner_heap[0x1000];
    extern char* fake_heap_start;
    extern char* fake_heap_end;

    // Configure the newlib heap.
    fake_heap_start = inner_heap;
    fake_heap_end   = inner_heap + sizeof(inner_heap);
}

// Service initialization.
void __appInit(void) {
    Result rc{};

    // Open a service manager session.
    if (R_FAILED(rc = smInitialize()))
        fatalThrow(rc);

    if (R_FAILED(rc = fsInitialize()))
        fatalThrow(rc);
    fsdevMountSdmc();

    // Close the service manager session.
    smExit();
}

// Service deinitialization.
void __appExit(void) {
    fsdevUnmountAll();
    fsExit();
}

} // extern "C"
