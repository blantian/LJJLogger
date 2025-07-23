package com.mgtv.mglogger.log.utils;

import java.io.ByteArrayOutputStream;
import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * This class contains various utilities to manipulate I/O. The methods of this
 * class,
 * although static, are not thread safe and cannot be invoked by several threads
 * at the
 * same time. Synchronization is required by the caller.
 */
public class IOUtils {
    public static final int DEFAULT_BUFFER_SIZE = 4 * 1024;

    /**
     * Opens a {@link FileInputStream} for the specified file, providing better
     * error
     * messages than simply calling <code>new FileInputStream(file)</code>
     * <p>
     * At the end of the method either the stream will be successfully opened,
     * or an
     * exception will have been thrown.
     * <p>
     * An exception is thrown if the file does not exist. An exception is thrown
     * if the file
     * object exists but is a directory. An exception is thrown if the file
     * exists but
     * cannot be read.
     * @param file
     *            the file to open for input, must not be <code>null</code>
     * @return a new {@link FileInputStream} for the specified file
     * @throws FileNotFoundException
     *             if the file does not exist
     * @throws IOException
     *             if the file object is a directory
     * @throws IOException
     *             if the file cannot be read
     */
    public static FileInputStream openInputStream(File file)
            throws IOException, FileNotFoundException {
        if (file.exists()) {
            if (file.isDirectory()) {
                throw new IOException(
                        "File '" + file + "' exists but is a directory");
            }
            if (!file.canRead()) {
                throw new IOException("File '" + file + "' cannot be read");
            }
        } else {
            throw new FileNotFoundException(
                    "File '" + file + "' does not exist");
        }
        return new FileInputStream(file);
    }

    /**
     * Opens a {@link FileOutputStream} for the specified file, checking and
     * creating the
     * parent directory if it does not exist.
     * <p>
     * At the end of the method either the stream will be successfully opened,
     * or an
     * exception will have been thrown.
     * <p>
     * The parent directory will be created if it does not exist. The file will
     * be created
     * if it does not exist. An exception is thrown if the file object exists
     * but is a
     * directory. An exception is thrown if the file exists but cannot be
     * written to. An
     * exception is thrown if the parent directory cannot be created.
     * @param file
     *            the file to open for output, must not be <code>null</code>
     * @return a new {@link FileOutputStream} for the specified file
     * @throws IOException
     *             if the file object is a directory
     * @throws IOException
     *             if the file cannot be written to
     * @throws IOException
     *             if a parent directory needs creating but that fails
     */
    public static FileOutputStream openOutputStream(File file)
            throws IOException {
        if (file.exists()) {
            if (file.isDirectory()) {
                throw new IOException(
                        "File '" + file + "' exists but is a directory");
            }
            if (!file.canWrite()) {
                throw new IOException(
                        "File '" + file + "' cannot be written to");
            }
        } else {
            File parent = file.getParentFile();
            if (parent != null && !parent.exists()) {
                if (!parent.mkdirs()) {
                    throw new IOException(
                            "File '" + file + "' could not be created");
                }
            }
        }
        return new FileOutputStream(file);
    }

    /**
     * Copy the content of the input stream into the output stream, using a
     * temporary byte
     * array buffer whose size is defined by {@link #DEFAULT_BUFFER_SIZE}.
     * @param in
     *            The input stream to copy from.
     * @param out
     *            The output stream to copy to.
     * @throws IOException
     *             If any error occurs during the copy.
     */
    public static void copy(InputStream in, OutputStream out)
            throws IOException {
        byte[] buffer = new byte[DEFAULT_BUFFER_SIZE];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    /**
     * Copy the content of the input stream into the output stream, using a
     * temporary byte
     * array buffer whose size is defined by {@link #DEFAULT_BUFFER_SIZE}.
     * @param in
     *            The input stream to copy from.
     * @param out
     *            The output stream to copy to.
     * @param length
     *            number of bytes to copy
     * @throws IOException
     *             If any error occurs during the copy.
     */
    public static void copy(InputStream in, OutputStream out, long length)
            throws IOException {
        byte[] buffer = new byte[DEFAULT_BUFFER_SIZE];
        int read;
        while ((read = in.read(buffer)) != -1 && length > 0) {
            out.write(buffer, 0, read);
            length -= read;
        }
    }

    /**
     * Closes the specified stream.
     * @param stream
     *            The stream to close.
     */
    public static void closeStream(Closeable stream) {
        if (stream != null) {
            try {
                stream.close();
            } catch (Exception e) {
                // NOTE(shituzheng):在android2.3之前的版本会有一个bug，网络状态不好的情况下可能会会抛出一个
                // NullPointerException，在之后的android中已经得到解决，具体原因暂未查明(查明后再补充)。
                // 此处捕获所有异常，避免crash。
                e.printStackTrace();
            }
        }
    }

    /**
     * Read file into buffer.
     * @param file
     *            the file to read, must not be null
     * @param offset
     *            position to read from
     * @param length
     *            length to read
     * @return the file contents, never null
     * @throws IOException
     *             - in case of an I/O error
     */
    public static byte[] readFileToByteArray(File file, long offset,
            long length) throws IOException {
        InputStream in = null;
        ByteArrayOutputStream out = null;
        try {
            in = openInputStream(file);
            out = new ByteArrayOutputStream();
            in.skip(offset);
            copy(in, out, length);
            return out.toByteArray();
        } finally {
            closeStream(in);
            closeStream(out);
        }
    }

    /**
     * get bytes from the assets file
     *
     * @param fileName
     * @return
     */
//    public static byte[] getBytesFromAssets(String fileName) {
//        byte[] readBuffer = new byte[DEFAULT_BUFFER_SIZE];
//        ByteArrayOutputStream baos = new ByteArrayOutputStream();
//        InputStream is = null;
//        try {
//            int bytesRead;
//            is = ContextProvider.getApplicationContext().getAssets()
//                    .open(fileName, MODE_PRIVATE);
//            while ((bytesRead = is.read(readBuffer)) != -1) {
//                baos.write(readBuffer, 0, bytesRead);
//            }
//            return baos.toByteArray();
//        } catch (IOException e) {
//            e.printStackTrace();
//            return null;
//        } finally {
//            closeStream(is);
//            closeStream(baos);
//        }
//    }
}
