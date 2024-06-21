package game.view;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;

import javax.swing.ImageIcon;
import javax.swing.JPanel;

/**
 * A class representing the graphic form of a wall
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public class WallView extends JPanel {
    /** Sprite for the walls */
    private Image sprite;

    /**
     * Constructor for the wall view
     */
    public WallView() {
        setOpaque(false);
        setPreferredSize(new Dimension(20, 20));
        sprite = new ImageIcon("lib/sprites/game/wall.png").getImage();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.drawImage(sprite, 0, 0, getWidth(), getHeight(), this);
    }
}
