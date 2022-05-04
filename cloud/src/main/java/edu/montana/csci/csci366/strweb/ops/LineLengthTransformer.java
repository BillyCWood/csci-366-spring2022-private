package edu.montana.csci.csci366.strweb.ops;

import java.util.concurrent.CountDownLatch;

/**
 * This class is should calculate the length of each line and replace the line in the
 * array with a string representation of its length
 */
public class LineLengthTransformer {
    String[] _lines;

    public LineLengthTransformer(String strings) {
        _lines = strings.split("\n");
    }

    public String toLengths() {

        //CountDownLatch tells the program to wait for the pool of threads
        //to complete before moving on
        CountDownLatch latch = new CountDownLatch(_lines.length);

        for(int i = 0; i < _lines.length; i++){
            String line = _lines[i];
           LineLengthCalculator lineLengthCalculator = new LineLengthCalculator(i,latch);
           new Thread(lineLengthCalculator).start();//create a new thread and execute it
        }
        try {
            latch.await(); //wait for all threads to complete
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        return String.join("\n",_lines);
    }

    class LineLengthCalculator implements Runnable{

        private final int index;
        private final CountDownLatch latch;

        public LineLengthCalculator(int index, CountDownLatch latch){
            this.index = index;
            this.latch = latch;
        }

        @Override
        public void run(){
            String currentValues = _lines[index];
            _lines[index] = String.valueOf(currentValues.length());
            latch.countDown(); //let all threads proceed
        }

    }

}
