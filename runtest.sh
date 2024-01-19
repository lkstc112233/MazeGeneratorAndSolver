#!/bin/bash

rm -f bfs-result dfs-result a-result
for i in 1 2 4 8 16
do
    for j in 1 2 4 8 16
    do
        for m in `seq 100 100 4000`
        do
            for n in `seq 100 100 4000`
            do
            	echo Now testing n = ${n}, m = ${m}, i = ${i} , j = ${j}
            	"./MazeGen" Maze-${n}-${m}-${i}-${j}.bmp ${n} ${m} ${i} ${j}
				echo -n ${n}\	 >> bfs-result
				echo -n ${m}\	 >> bfs-result
				"./BFS" Maze-${n}-${m}-${i}-${j}.bmp Maze-${n}-${m}-${i}-${j}-BFS.bmp >> bfs-result
				echo -n ${n}\	 >> dfs-result
				echo -n ${m}\	 >> dfs-result
				"./DFS" Maze-${n}-${m}-${i}-${j}.bmp Maze-${n}-${m}-${i}-${j}-DFS.bmp >> dfs-result
				echo -n ${n}\	 >> a-result
				echo -n ${m}\	 >> a-result
				"./AAsterisk" Maze-${n}-${m}-${i}-${j}.bmp Maze-${n}-${m}-${i}-${j}-a.bmp >> a-result
            done
        done
    done
done
