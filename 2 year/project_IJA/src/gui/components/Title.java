package gui.components;

import java.awt.Color;

import javax.swing.JLabel;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;

import common.Config;

/**
 * This class is configured JLabel that is used
 * as Title on each page.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class Title extends JLabel {
    /** config of the game */
    private static final Config config = new Config();

    /**
     * Creates custom title component
     * 
     * @param text title text
     */
    public Title(String text) {
        super(text);
        setFont(config.getFont("CrackMan.TTF", 54));
        Border border = getBorder();
        Border margin = new EmptyBorder(10, 10, 10, 10);
        setBorder(new CompoundBorder(border, margin));
        setForeground(Color.WHITE);
    }
}
