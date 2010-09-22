Introduction
============

SigViewer is a powerful viewing application for biosignals, 
originally designed to display electroencephalographic (EEG) data.


.. _supported-file-formats:


Commandline Arguments
^^^^^^^^^^^^^^^^^^^^^
Usage: ``sigviewer [OPTION] [FILE]``

Options:
 
-t, --test	starts SigViewer in testing mode
-c, --convert-to-gdf	converts the input file into gdf and saves it to the file given by the option '-o'
-o FILE, --output-file FILE	output file name for converted file

Remark: Converting to GDF is in an experimental state!!!

Supported File Formats
^^^^^^^^^^^^^^^^^^^^^^
Generally, all file formats which can be read by the biosig library can be handeled by SigViewer.

However, the primary file format is GDF 2 [Sch99]_.

.. [Sch99] Schlögl A.: GDF - A general dataformat for biosignals. http:///arxiv.org/abs/cs.DB/0608052