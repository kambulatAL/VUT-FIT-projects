package game.view;

import java.awt.Graphics;
import java.awt.Image;

import javax.swing.ImageIcon;

/**
 * A class representing the graphic form of a ghost
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public class GhostView implements ComponentView {
    /** field where ghost is */
    private FieldView field;
    /** sprite for the ghost */
    private Image sprite;

    /**
     * Constructor for the ghost
     * @param field View of the field
     */
    public GhostView(FieldView field) {
        this.field = field;
        sprite = new ImageIcon("lib/sprites/game/ghost.png").getImage();
    }

    @Override
    public void paintComponent(Graphics g) {
        g.drawImage(sprite, 0, 0, field.getWidth(), field.getHeight(), field);
    }
}
