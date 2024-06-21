package common;

import game.common.CommonField;
import game.objects.PacmanObject;
import game.objects.PathField;

import java.util.*;

/**
 * A class that calculates algorithm A*.
 * Each function here is used to properly calculate path to a destination point
 * 
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class AStar {
    private static class Node {
        CommonField field;
        Node parent;
        int distanceFromStart;
        int distanceToDest;
        CommonField.Direction dir;

        public Node(CommonField field, Node parent) {
            this.field = field;
            this.parent = parent;
            if (parent != null) {
                this.distanceFromStart = parent.distanceFromStart + 1;
            } else {
                this.distanceFromStart = 0;
            }
        }

        public void setDir(CommonField.Direction dir) {
            this.dir = dir;
        }

        public int getDistance() {
            return this.distanceToDest;
        }
    }

    private CommonField destField;
    private PacmanObject pacman;
    private List<Node> open = new ArrayList<>();
    private List<Node> close = new ArrayList<>();
    private List<CommonField.Direction> path = new ArrayList<>();

    /**
     * Contruct an Astar object
     * @param field represents a field of the maze
     */
    public AStar(CommonField field) {
        this.destField = field;
        this.pacman = field.getMaze().getPacman();
    }

    /**
     * Calculates a heuristic based on the Manhattan distance for each node of the algorithm
     * @param field represents a field of the maze
     * @return distance to the goal field
     */
    public int calcHeuristic(CommonField field) {
        int fieldX = field.getX();
        int fieldY = field.getY();
        int ghostX = destField.getX();
        int ghostY = destField.getY();
        return Math.abs(fieldX - ghostX) + Math.abs(fieldY - ghostY);

    }

    /**
     * initialize and adds new node to the list of the nodes
     * @param field represents a field of the maze
     * @param parent parent node, which leads to the new one
     * @param dir is a direction, that leads to this node
     */
    public void addNewOpenNode(CommonField field, Node parent, CommonField.Direction dir) {
        Node tmp = new Node(field, parent);
        tmp.distanceToDest = this.calcHeuristic(field) + tmp.distanceFromStart;
        tmp.setDir(dir);
        this.open.add(tmp);
    }


    /**
     * Finds node with the smallest distance value
     * @return  distance to the target point
     */
    public Node getSmallest() {
        return Collections.min(this.open, Comparator.comparing(Node::getDistance));
    }

    /**
     * Checks if the coordinates of two fields are equal
     * @param field1 represents a field of the maze
     * @param field2 represents a field of the maze
     * @return  boolean value of the checking
     */
    public boolean equalCoords(CommonField field1, CommonField field2) {
        return (field1.getX() == field2.getX() && field1.getY() == field2.getY());
    }

    /**
     * Checks if the field for the new potential node is in the Close list.
     * @param field represents a field of the maze
     * @return  boolean value of the checking
     */
    public boolean checkIfInClose(CommonField field) {
        for (Node node : this.close) {
            if (this.equalCoords(field, node.field)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if a field on the certain direction is passable
     * @param dir is a direction, that leads to the next field
     * @param field represents a field of the maze
     * @return  boolean value of the checking
     */
    public boolean canMove(CommonField.Direction dir, CommonField field) {
        return field.nextField(dir) instanceof PathField;
    }

    /**
     * Expands the node to new ones in all directions
     * @param parent previous node of a potential new node to expand
     */
    public void expandNode(Node parent) {
        List<CommonField.Direction> directions = Arrays.asList(CommonField.Direction.R, CommonField.Direction.L,
                CommonField.Direction.U, CommonField.Direction.D);

        for (CommonField.Direction dir : directions) {
            if (this.canMove(dir, parent.field)) {
                CommonField field = (PathField) parent.field.nextField(dir);
                if (field != null && !this.checkIfInClose(field)) {
                    this.addNewOpenNode(field, parent, dir);
                }
            }
        }
    }

    /**
     * Get directions form the start node to the target one and save those to the list
     * @param node the target node
     */
    public void getPath(Node node) {
        Node tmp = node;
        while (tmp.parent != null) {
            this.path.add(tmp.dir);
            tmp = tmp.parent;
        }
        Collections.reverse(this.path);
    }

    /**
     * Searches an optimal way from the start node to the target one
     */
    public void FindPath() {
        this.addNewOpenNode(this.pacman.field, null, null);

        while (!this.open.isEmpty()) {
            Node smallest = this.getSmallest();

            if (this.equalCoords(smallest.field, this.destField)) {
                this.getPath(smallest);
                return;
            }
            this.expandNode(smallest);

            this.open.remove(smallest);
            this.close.add(smallest);
        }
    }

    /**
     * Starts the A* algorithm and leads Pacman to the target field
     * @throws InterruptedException InterruptedException
     */
    public void startAStar() throws InterruptedException {
        this.FindPath();
        if (this.path.isEmpty())
            return;
        if (this.pacman.getField().getMaze().getPause())
            return;

        for (CommonField.Direction dir : this.path) {
            this.pacman.move(dir, false);
            Thread.sleep(250);
        }

    }
}
