Using SigViewer
===============

.. |View Options Icon| image:: ../../src/images/icons/configure.png
.. |New Event Icon| image:: ../../src/images/new_22x22.png
.. |Edit Event Icon| image:: ../../src/images/pointer_22x22.png
.. |Scroll Icon| image:: ../../src/images/hand_22x22.png
.. |Open Icon| image:: ../../src/images/icons/fileopen.png
.. |Delete Event Icon| image:: ../../src/images/icons/editdelete.png
.. |Change Channel Icon| image:: ../../src/images/change_channel_22x22.png
.. |Change Type Icon| image:: ../../src/images/change_type_22x22.png

Basics
------

Commandline Arguments
^^^^^^^^^^^^^^^^^^^^^
Usage: ``sigviewer [OPTION] [FILE]``

Options:
 
-t, --test	starts SigViewer in testing mode
-c, --convert-to-gdf	converts the input file into gdf and saves it to the file given by the option '-o'
-o FILE, --output-file FILE	output file name for converted file

Remark: Converting to GDF is in an experimental state!!!

Modes
^^^^^

SigViewer supports different mouse modes. Each mode leads to different
behavior of the mouse in the signal view widget.

|New Event Icon| New Event
  Inserting new events. 

|Edit Event Icon| Edit Event
  Selecting and editing events.

|Scroll Icon| Scroll
  Scrolling the viewport.

|View Options Icon| View Options
  Adapting the viewport.


.. include:: using_open_files.rst
.. include:: using_adapting_signal_view.rst
.. include:: using_editing_events.rst






