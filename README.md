[//]: # (Markdown: dillinger.io/ shows a nice example of Markdown commands with a viewer.)
[//]: # (Comments in Markdown: http://stackoverflow.com/questions/4823468/comments-in-markdown)
[//]: # (C++ Project Structure: http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/)
[//]: # (C++ Library Creation: http://www.adp-gmbh.ch/cpp/gcc/create_lib.html)

# Xperimenter

Xperimenter is a workflow management tool.
Xperimenter manages a workflow tree. In a workflow tree, multiple workflows
share tasks common to them, thus eliminating running the same task multiple
times. Xperimenter is also multi-threading: All the workflows in the tree can
be run simultaneously.

The results from running a workflow tree are stored in a directory structure,
where each directory contains a task. Each directory contains a params.csv
file, which stores the parameters specifying the task. The results from running
the task are also contained in this directory.

A workflow tree is known as a project. The project is stored in project.xml
in its own directory.

Requirements

1)	C++ Boost libraries
	To install: From a shell, type "sudo port -v install boost"

2)	Boost.Process
	Download from http://www.highscore.de/boost/process/ and unzip
	Copy boost/process and boost/process.hpp to /opt/local/include/boost
	Copy libs/process to /opt/local/lib

3)	wxWidgets
	To install: From a shell, type "sudo port -v install wxwidgets"

### Installation

```sh
$ git clone git@github.com:jcasse/xperimenter.git
$ cd xperimenter
$ make
```
### Usage

```sh
$ bin/blockmodel-visualization <input dir>
To quit: press 'esc' key

Example:

main ~/Research/datasets/multi/binary_3d/multi-clustering/multi-clustering_20120901090553/
```

License
----

[//]: # (A short snippet describing the license (MIT, Apache, etc.))

[//]: # (http://choosealicense.com/)

Copyright (C) 2016 Juan Casse

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
