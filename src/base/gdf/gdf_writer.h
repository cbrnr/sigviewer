// gdf_writer.h

#ifndef GDF_WRITER_H
#define GDF_WRITER_H

#include "../file_signal_writer.h"
#include "gdf_1_header.h"

#include <QFile>

namespace BioSig_
{

// GDF writer
class GDFWriter : public FileSignalWriter,
                  private GDF1Header
{
public:
    GDFWriter();
    virtual ~GDFWriter();
    virtual FileSignalWriter* clone();

    virtual QString save(FileSignalReader& file_signal_reader,
                      SignalEventVector& event_vector,
                      const QString& file_name,
                      bool save_signals = true);

private:
    // not allowed
    GDFWriter(const GDFWriter& src);
    const GDFWriter& operator=(const GDFWriter& src);

    void saveFixedHeader(bool save_signals);
    void saveSignalHeaders();
    bool saveSignals();
    void saveEvents(SignalEventVector& event_vector);

    QFile file_;
    FileSignalReader* file_signal_reader_;
};

} // namespace BioSig_

#endif
