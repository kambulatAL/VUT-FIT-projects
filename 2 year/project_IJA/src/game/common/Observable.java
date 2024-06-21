package game.common;

/**
 * Observable interface representing objects that can notify dependent objects
 * (observers) of changes. MazeObjects can be inserted.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public interface Observable {
    /**
     * Registers a new observer.
     * 
     * @param o registered observer
     */
    void addObserver(Observable.Observer o);

    /**
     * Removes the observer
     * 
     * @param o Observer to be removed from the list of registered observers.
     */
    void removeObserver(Observable.Observer o);

    /**
     * Notifies (informs) registered observers that the state of the object has
     * changed.
     */
    void notifyObservers();

    /**
     * Observer interface representing objects that can register with Observable
     * objects and receive notifications about their changes.
     */
    public interface Observer {
        /**
         * Processes a notification about a change in an Observable object.
         * 
         * @param o The object in which the change occurred.
         */
        void update(Observable o);
    }
}
