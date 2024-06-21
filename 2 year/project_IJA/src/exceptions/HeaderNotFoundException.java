package exceptions;

/**
 * Class that implements custome exception for maze file reading.
 * Game throws this type of error when maze file has invalid header
 * 
 * @author Oleksandr Turytsia (xalaka00)
 * @version 1.0
 */
public class HeaderNotFoundException extends Exception {
    /**
     * Exception throws if a map file has its size
     */
    public HeaderNotFoundException() {
        super();
    }

    /**
     * Exception throws if a map file has its size
     * @param message message of the exception
     */
    public HeaderNotFoundException(String message) {
        super(message);
    }
}
