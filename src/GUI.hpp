#pragma once
#ifndef GUI_HPP
#define GUI_HPP

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Buildings/Building.hpp"
#include "Buildings/Commercial/Commercial.hpp"
#include "Buildings/Industrial.hpp"
#include "Buildings/Residential.hpp"
#include "Cars/Car.hpp"
#include "Cars/Resident.hpp"
#include "City.hpp"
#include "Roads/Intersection.hpp"
#include "Roads/Road.hpp"
#include "Utilities.hpp"

/**
 * @brief Enumeration representing different tools for the GUI.
 */
enum class Tool {
  None,
  Delete,
  AddRoad,
  Residential,
  Commercial,
  Industrial,
  Pause,
  FastForward,
  FastForward10x,
  Road30,
  Road40,
  Road60,
  IntersectionLights,
  IntersectionNoLights
};

// Forward declaration of the City class
class City;

/**
 * @brief Represents the GUI for interacting with the City.
 */
class GUI {
public:
  /**
   * @brief Constructor for the GUI class.
   * @param city Reference to the City object.
   * @param size Size parameter for the GUI representing the size of the City object.
   */
  GUI(City &city, int size);

  /**
   * @brief Initializes the window and all the elements needed for the GUI.
   */
  void init();

  /**
   * @brief Handles all the possible events in the GUI.
   */
  void handleEvent();

  /**
   * @brief Draws and displays the GUI.
   */
  void drawAndDisplay();

  /**
   * @brief Checks if the GUI window is open.
   * @return True if the window is open, false otherwise.
   */
  bool isOpen() { return window_.isOpen(); }

  /**
   * @brief Checks if the simulation is paused.
   * @return True if paused, false otherwise.
   */
  bool isPaused() { return paused_; }

  /**
   * @brief Checks if the city has changed since the last time the simulation paused.
   * @return True if the city has changed, false otherwise.
   */
  bool hasCityChanged() { return cityChanged_; }

  /**
   * @brief Sets the changed status of the city.
   * @param val New value for the cityChanged flag.
   */
  void setChanged(bool val) { cityChanged_ = val; }

  /**
   * @brief Gets the current tool type selected in the GUI.
   * @return The current tool type.
   */
  Tool currentToolType() { return currentTool_; }

  /**
   * @brief Opens another window and draws a histogram on that window based on the data.
   * @param data Vector of data for the histogram.
   * @param yLabel Label for the y-axis.
   * @param type Type parameter for the histogram. 1 is for Road histogram, 2 is for City and 3 is
   * for Intersection.
   */
  void drawHistogram(std::vector<double> data, std::string yLabel, int type);

  /**
   * @brief Reads a City from a CSV file.
   * @param filename Name of the CSV file to read. This is "City.csv" in the program.
   */
  void readCSV(const std::string &filename);

  /**
   * @brief Writes the City to a CSV file.
   * @param city Reference to the City object.
   * @param filename Name of the CSV file to write. This is "City.csv" in the program.
   */
  void writeCSV(City &city, const std::string &filename);

private:
  // boolean values needed for GUI
  bool isAddBuildingButtonPressed_;
  bool isAddRoadButtonPressed_;
  bool isPopUp_;
  bool paused_;
  bool cityChanged_;
  bool isInputBoxVisible_;
  bool roadPopUp_;
  bool histIsOpen_;
  bool isHeatMap_;

  // Button colors
  sf::Color defaultButtonColor_ = sf::Color::White;
  sf::Color pressedButtonColor_ = sf::Color::Black;

  // Miscellaneous variables needed for GUI
  Tool currentTool_;
  int residentCount_;
  size_t size_;
  City &city_;
  float cellSize_;
  sf::Font font_;
  std::string userInput_;

  // Main window and histogram window
  sf::RenderWindow window_;
  sf::RenderWindow histogramWindow_;

  // All the different Text objects needed for the GUI
  sf::Text deleteText_, addBuildingText_, addRoadText_, heatMapText_, pauseText_, fastForward_,
      road30Text_, road40Text_, road60Text_, intersectionNoLightsText_, intersectionLightsText_,
      residentialText_, commercialText_, industrialText_, inputText_, time_, popUpText_, histText_,
      analysisText_, cityHistText_, exportDataText_;

  // All the RectangleShape objects needed for the GUI
  std::vector<sf::RectangleShape> staticRectangles_;
  sf::RectangleShape dropdown_, exportDataButton_, backdrop_, cityHistButton_, histButton_, inputBox_, popUp_;

  // Current data for drawHistogram function
  int currentHistType_;
  std::vector<double> currentStats_;
  std::string currentHistLabel_;
};

#endif