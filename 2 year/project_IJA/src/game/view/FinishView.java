package game.view;

import java.awt.Graphics;
import java.awt.Image;

import javax.swing.ImageIcon;

/**
 * A class representing the graphic form of a finish
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public class FinishView implements ComponentView {
    /** Field where finish is */
    private FieldView field;
    /** Sprite for the finish */
    private Image sprite;

    /**
     * Sets view to the finish field
     * @param field FieldView object
     */
    public FinishView(FieldView field) {
        this.field = field;
        sprite = new ImageIcon("lib/sprites/game/finish.png").getImage();
    }

    @Override
    public void paintComponent(Graphics g) {
        g.drawImage(sprite, 0, 0, field.getWidth(), field.getHeight(), field);
    }
}
