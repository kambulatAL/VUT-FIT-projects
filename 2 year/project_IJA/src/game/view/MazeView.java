package game.view;

import java.awt.GridLayout;

import javax.swing.JPanel;

import game.common.CommonField;
import game.objects.Maze;
import gui.Game;

/**
 * A class representing the graphic form of a maze
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public class MazeView extends JPanel {
    /**
     * constructs graphic representation of a maze
     * 
     * @param maze maze abstractions
     * @param game game to which this graphic maze will be bound
     */
    public MazeView(Maze maze, Game game) {
        setLayout(new GridLayout(maze.numRows(), maze.numCols()));
        setOpaque(false);
        for (int i = 0; i < maze.numRows(); ++i) {
            for (int j = 0; j < maze.numCols(); ++j) {
                CommonField field = maze.getField(j, i);

                if (field.canMove()) {
                    add(new FieldView(field, game));
                } else {
                    add(new WallView());
                }
            }
        }
    }
}
