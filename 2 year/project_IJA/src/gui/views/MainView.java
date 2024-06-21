package gui.views;

import java.awt.GridLayout;
import java.awt.LayoutManager;

import javax.swing.JPanel;

import gui.Game;
import gui.components.Background;
import gui.components.Button;

/**
 * This class represents default view for the
 * game when it first launched (menu interface)
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class MainView extends View {

    /* PANELS */
    /** grid layout */
    private final LayoutManager gridLayout = new GridLayout(3, 1);
    /** button container */
    private final JPanel buttonContainer = new JPanel(gridLayout);

    /* BUTTONS */
    /** button for the start game */
    private final Button buttonStart = new Button("Start game");
    /** button load */
    private final Button buttonLoad = new Button("Load replay");
    /** button exit */
    private final Button buttonExit = new Button("Exit");

    /**
     * Creates starting game
     * 
     * @param game game
     */
    public MainView(Game game) {
        super(game);

        buttonContainer.setBounds(config.getWidth() - 20 - 200 - 16, 20, 200, 3 * 50);
        buttonContainer.setOpaque(false);

        buttonStart.addActionListener(e -> {
            game.pushView(new StartGameView(game));
        });

        buttonLoad.addActionListener(e -> {
            game.pushView(new LoadGameView(game));
        });

        buttonExit.addActionListener(e -> {
            game.dispose();
        });

        buttons.add(buttonStart);
        buttons.add(buttonLoad);
        buttons.add(buttonExit);

        for (Button button : buttons) {
            buttonContainer.add(button);
        }

        selectButton(activeButton);

        add(buttonContainer);
        add(new Background("lib/sprites/menu/bg-menu.jpg"));
    }

    @Override
    protected void KeyArrowLeft() {
    }

    @Override
    protected void KeyArrowUp() {
        selectPrevButton();
        game.update();
    }

    @Override
    protected void KeyArrowRight() {
    }

    @Override
    protected void KeyArrowDown() {
        selectNextButton();
        game.update();
    }

    @Override
    protected void KeyEscape() {
    }

    @Override
    protected void KeyEnter() {
        buttons.get(activeButton).doClick();
    }

    @Override
    protected void AnyKey() {
    }
}
