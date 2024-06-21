package gui.components;

import java.awt.Color;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;

import common.Config;

/**
 * Component that represents button in the game
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class Button extends JButton {
    /** config of the game */
    private final static Config config = new Config();
    /** True if button is focused */
    private boolean hasSelect;
    /** True if button is hovered */
    private boolean isActive;

    /**
     * Initialize a button object
     * @param text text for the button
     */
    public Button(String text) {
        super(text);
        setFont(config.getFont("emulogic.ttf", 10f));
        setForeground(Color.WHITE);
        setOpaque(false);
        setContentAreaFilled(false);
        setFocusPainted(false);
        setBorderPainted(false);
        setFocusable(false);

        addMouseListener((MouseListener) new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
            }

            @Override
            public void mouseEntered(MouseEvent e) {
                if (!hasSelect && !isActive) {
                    setBorderPainted(true);
                    setBorder(BorderFactory.createDashedBorder(Color.WHITE, 5, 5));
                }
            }

            @Override
            public void mouseExited(MouseEvent e) {
                if (!hasSelect && !isActive) {
                    setBorderPainted(true);
                    setBorder(null);
                }
            }
        });
    }

    /**
     * Setter for the isActive attribute
     * @param active boolean value to be set to the isActive attribute
     */
    public void setActive(boolean active) {
        this.isActive = active;

        setBorderPainted(active);
        Border outerBorder = BorderFactory.createLineBorder(Color.WHITE, 3);
        Border gapBorder = BorderFactory.createEmptyBorder(2, 2, 2, 2);
        Border innerBorder = BorderFactory.createLineBorder(Color.WHITE, 2);
        setBorder(BorderFactory.createCompoundBorder(outerBorder, new CompoundBorder(gapBorder,
                innerBorder)));
    }

    /**
     * Getter that for the isActive attribute
     * @return isActive attribute
     */
    public boolean getActive() {
        return this.isActive;
    }

    /**
     * Creates a broken line around the button if it was selected
     * @param hasSelect boolean value specifies if the button was selected
     */
    public void setSelect(boolean hasSelect) {
        if (isActive)
            return;

        this.hasSelect = hasSelect;
        setBorderPainted(hasSelect);
        setBorder(BorderFactory.createLineBorder(Color.WHITE, 2, false));

    }
}
