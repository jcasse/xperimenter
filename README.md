## Synopsis

[//]: # (At the top of the file there should be a short introduction and/ or overview that explains **what** the project is. This description should match descriptions added for package managers (Gemspec, package.json, etc.))

Xperimenter is a workflow management tool.
Xperimenter manages a workflow tree. In a workfloe tree, multiple workflows
share tasks common to them, thus eliminating running the same task multiple
times. Xperimenter is also multi-threading: All the workflows in the tree can
be run simultaneously.

The results from running a workflow tree are stored in a directory structure,
where each directory contains a task. Each directory contains a params.csv
file, which stores the parameters specifying the task. The results from running
the task are also contained in this directory.

A workflow tree is known as a project. The project is stored in project.xml
in its own directory.

## Code Example

[//]: # (Show what the library does as concisely as possible, developers should be able to figure out **how** your project solves their problem by looking at the code example. Make sure the API you are showing off is obvious, and that your code is short and concise.)

## Motivation

[//]: # (A short description of the motivation behind the creation and maintenance of the project. This should explain **why** the project exists.)

This application was created to help iterater over experimental runs faster,
and to keep them organized in a hierarchical directory structure.

## Installation

[//]: # (Provide code examples and explanations of how to get the project.)

Requirements

1)	C++ Boost libraries
	To install: From a shell, type "sudo port -v install boost"

2)	Boost.Process
	Download from http://www.highscore.de/boost/process/ and unzip
	Copy boost/process and boost/process.hpp to /opt/local/include/boost
	Copy libs/process to /opt/local/lib

3)	wxWidgets
	To install: From a shell, type "sudo port -v install wxwidgets"

Build

From a shell, cd into the source directory and type "make"

## API Reference

[//]: # (Depending on the size of the project, if it is small and simple enough the reference docs can be added to the README. For medium size to larger projects it is important to at least provide a link to where the API reference docs live.)

TODO

## Tests

[//]: # (Describe and show how to run the tests with code examples.)

TODO

## Contributors

[//]: # (Let people know how they can dive into the project, include important links to things like issue trackers, irc, twitter accounts if applicable.)

TODO

## License

[//]: # (A short snippet describing the license (MIT, Apache, etc.))

[//]: # (http://choosealicense.com/)

Copyright (C) 2015 Juan Casse

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

