package edu.montana.csci.csci366.strweb.ops;

import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;

/**
 * This class is a simple sorter that implements sorting a strings rows in different ways
 */
public class Sorter {
    private final String _strings;

    public Sorter(String strings) {_strings = strings;}

    //sort sequentially in ascending order using a single thread
    public String sort() {
        String[] split = _strings.split("\n|\r\n");
        Arrays.sort(split);
        return String.join("\n",split);
    }

    //opposite order of sort()
    public String reverseSort() {
        String[] split = _strings.split("\n|\r\n");
        Arrays.sort(split, Collections.reverseOrder());
        return String.join("\n",split);
    }

    //sort the data in ascending order using multiple threads available in the thread pool
    //uses Fork/Join framework
    public String parallelSort() {
        String[] split = _strings.split("\n|\r\n");
        Arrays.parallelSort(split);
        return String.join("\n",split);
    }
}
