package game.view;

import java.awt.Graphics;

/**
 * Object interfaces that can be nested under FieldView.
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public interface ComponentView {
    /**
     * Renders the graphic form of the object into the graphic context g.
     * 
     * @param g Graphics object
     */
    void paintComponent(Graphics g);
}
