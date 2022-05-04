package edu.montana.csci.csci366.strweb.ops;

import java.lang.reflect.GenericDeclaration;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;

/**
 * This class is should calculate the SHA 256 hexadecimal hash for each line and replace the line in the
 * array with that hash.
 *
 * It should do so using the ThreadPoolExecutor created below.
 */
public class Sha256Transformer {
    String[] _lines;
    ThreadPoolExecutor executor = (ThreadPoolExecutor) Executors.newFixedThreadPool(10);

    public Sha256Transformer(String strings) {
        _lines = strings.split("\n");
    }

    public String toSha256Hashes() {
        //TODO - this method should create a series of Runnables and use the executor to do all
        //       word counts in parallel
        CountDownLatch latch = new CountDownLatch(_lines.length);

        for(int i = 0; i < _lines.length; i++){
            Sha256Computer sha256Computer = new Sha256Computer(i, latch);
            executor.execute(sha256Computer);
        }
        try {
            latch.await(); //wait for all threads to complete
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        return String.join("\n",_lines);
    }

    class Sha256Computer implements Runnable{

        private final int index;
        private final CountDownLatch latch;

        public Sha256Computer(int index, CountDownLatch latch){
            this.index = index;
            this.latch = latch;
        }

        @Override
        public void run(){
            MessageDigest digest = null;
            try{
                String originalString = _lines[index];
                digest = MessageDigest.getInstance("SHA-256");
                byte[] encodedhash = digest.digest(
                        originalString.getBytes(StandardCharsets.UTF_8));
                _lines[index] = bytesToHex(encodedhash);
                latch.countDown();
            }catch (NoSuchAlgorithmException e){
                throw new RuntimeException(e);
            }

        }

        public String bytesToHex(byte[] hash){
            StringBuilder hexString = new StringBuilder(2*hash.length);
            for(int i = 0; i < hash.length;i++){
                String hex = Integer.toHexString(0xff & hash[i]);
                if(hex.length() == 1){
                    hexString.append('0');
                }
                hexString.append(hex);
            }
            return hexString.toString();
        }
    }

}
