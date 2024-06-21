package gui.components;

import java.awt.Color;

import javax.swing.JLabel;

import common.Config;

/**
 * This class is configured JLabel that is used
 * as simple text in the game.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class Label extends JLabel {
    /**config of the game */
    private static Config config = new Config();

    /**
     * Creates configured label component
     * 
     * @param text label text
     */
    public Label(String text) {
        this(text, 10);
    }

    /**
     * Creates configured label component with custom size
     * 
     * @param text label text
     * @param size size of the label
     */
    public Label(String text, float size) {
        super(text);
        setFont(config.getFont("emulogic.ttf", size));
        setForeground(Color.WHITE);
    }
}
