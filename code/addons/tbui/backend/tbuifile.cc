#include "io/ioserver.h"
#include "tbuifile.h"
#include "tb/tb_system.h"

namespace tb
{
// static
TBFile*
tb::TBFile::Open(const char* filename, TBFileMode mode)
{
    if (mode != TBFileMode::MODE_READ)
        return nullptr;

    Util::String resolvedFilename(filename);

    if (resolvedFilename.BeginsWithString("@TBUI"))
    {
        //resolvedFilename = ;???
    }

    TBUI::TBUIFile* file = new ::TBUI::TBUIFile(resolvedFilename);
    if (!file->IsOpen())
    {
        file = nullptr;
    }

    return file;
}
} // namespace tb

namespace TBUI
{
TBUIFile::TBUIFile(const Util::String& filePath)
    : fileStream()
{
    fileStream = IO::IoServer::Instance()->CreateStream(filePath).downcast<IO::FileStream>();
}

TBUIFile::~TBUIFile()
{
    if (fileStream->IsOpen())
        fileStream->Close();
}

long
TBUIFile::Size()
{
    return fileStream->GetSize();
}

size_t
TBUIFile::Read(void* buf, size_t elemSize, size_t count)
{
    return fileStream->Read(buf, count);
}
} // namespace TBUI
