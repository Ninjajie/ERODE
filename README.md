# ERODE
Maya Plugin for generating Hydrology-conformed rivers on arbitrary terrain mesh.

[Youtube Link](https://www.youtube.com/watch?v=tc0nj5eKV9E)

_Welcome to subscribe to my channel!_

## What's it about
The Plugin is based on a [SIGGRAPH2013 paper](https://dl.acm.org/citation.cfm?doid=2461912.2461996), the original paper dealt with
a bigger problem of generating an entire terrain from river network. To render its final terrian, the authors adopts CSGs to represent
terrain patches, which we think could be a problem.

However, we found the idea of generating hydrology-based river networks **given the terrain** could be interesting, so we did it! It becomes 
this project. We wanted to do it in Maya since we are familiar with it, and various Maya tools are also helpful along the way.

## To use
This GitHub Repo contains only source files, you could

* build it in Visual Studio 2015 by opening the solution and build
* after build, the plugin file Erode.mll would be generated under the folder
* open Maya2016, browse to the folder and load the plugin
* use the plugin as showed in the YouTube link above.
