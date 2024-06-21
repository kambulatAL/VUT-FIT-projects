package game.view;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;

import javax.swing.ImageIcon;

/**
 * A class representing the graphic form of a point
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public class PointView implements ComponentView {
    /** Field where score point is */
    private FieldView field;
    /** Sprite for the score point */
    private Image sprite;

    /**
     * Constructor for the point view
     * @param field field of the maze
     */
    public PointView(FieldView field) {
        this.field = field;
        sprite = new ImageIcon("lib/sprites/game/points.png").getImage();
        field.setPreferredSize(new Dimension(20, 20));
    }

    @Override
    public void paintComponent(Graphics g) {
        g.drawImage(sprite, 0, 0, field.getWidth(), field.getHeight(), field);
    }
}
