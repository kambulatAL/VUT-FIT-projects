package common;

import java.awt.Font;
import java.awt.FontFormatException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.List;
import java.util.Properties;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * Config is a class, that mostly manages configurational
 * data for the project that are stored at config.properties.
 * 
 * It can also import fonts and files.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class Config {
    private int width; // horizontal resolution (px)
    private int height; // vertical resolution (px)

    /**
     * Constructor for the config class
     */
    public Config() {
        Properties props = new Properties();

        try {
            props.load(new FileInputStream("data/config.properties"));

            width = Integer.parseInt(props.getProperty("window.width"));
            height = Integer.parseInt(props.getProperty("window.height"));

        } catch (FileNotFoundException e) {
            System.out.println(e.getMessage());
        } catch (IOException e) {
            System.out.println(e.getMessage());
        }
    }

    /**
     * Retuns horizontal resolution of the window
     * 
     * @return horizontal resolution in pixels
     */
    public int getWidth() {
        return width;
    }

    /**
     * Retuns vertical resolution of the window
     * 
     * @return vertical resolution in pixels
     */
    public int getHeight() {
        return height;
    }

    /**
     * Imports font, that are pre-installed for the project
     * 
     * @param font font's name
     * @param size font's size
     * @return requested fonts or null if error occured
     */
    public Font getFont(String font, float size) {
        try {
            return Font.createFont(Font.TRUETYPE_FONT, new File("lib/fonts/" + font)).deriveFont(size);
        } catch (IOException | FontFormatException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * Returns all the files from given directory
     * 
     * @param dir path to the directory
     * @return List of files that are in directory
     */
    public List<File> getFiles(String dir) {
        try {
            return Stream.of(new File(dir).listFiles())
                    .filter(file -> !file.isDirectory())
                    .collect(Collectors.toList());
        } catch (NullPointerException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * Returns all map files that exist in the game
     * 
     * @return list of files
     */
    public List<File> getMaps() {
        return getFiles("data/maps");
    }
}
