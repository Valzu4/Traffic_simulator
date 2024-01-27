#include "GUI.hpp"
#include <iomanip>
#include <iostream>

void GUI::readCSV(const std::string &filename) {
  std::ifstream csvFile(filename);

  if (!csvFile.is_open()) {
    std::cerr << "Failed to open the CSV file for reading." << std::endl;
    return;
  }
  std::string line;
  std::string cell;
  // Skip the header row
  std::getline(csvFile, line);
  // Size line:
  std::getline(csvFile, line);
  int size = std::stoi(line);
  city_.clearAndResize(size);

  // Read each row and populate the City
  while (std::getline(csvFile, line)) {
    std::istringstream lineStream(line);
    size_t x, y;
    std::string squareType;
    int data;

    // Read the values from the CSV line
    std::getline(lineStream, cell, ',');
    x = std::stoi(cell);
    std::getline(lineStream, cell, ',');
    y = std::stoi(cell);
    std::getline(lineStream, squareType, ',');
    std::getline(lineStream, cell, ',');
    if (cell == "-") {
      data = 0; // Set resident count to 0 for placeholder
    } else {
      data = std::stoi(cell);
    }
    // Populate the City with the square based on the CSV data
    Position pos(x, y);
    if (squareType == "Residential") {
      Residential *square = new Residential(pos, city_, data);
      city_.add(pos, square);
    } else if (squareType == "Industrial") {
      Industrial *square = new Industrial(pos, city_, data);
      city_.add(pos, square);
    } else if (squareType == "Commercial") {
      Commercial *square = new Commercial(pos, city_, data);
      city_.add(pos, square);
    } else if (squareType == "Road") {
      Road *square = new Road(pos, city_, data);
      city_.add(pos, square);
    } else if (squareType == "Intersection") {
      std::vector<Direction> empty = {Direction::N, Direction::S, Direction::E,
                                      Direction::W}; // Add all directions to prevent errors
      Intersection *square = new Intersection(pos, city_, empty, data);
      city_.add(pos, square);
    } else {
      city_.add(pos, new Square(pos, city_));
    }
  }

  csvFile.close();
  std::cout << "City data has been read from " << filename << std::endl;
  setChanged(true);
}

void GUI::writeCSV(City &city, const std::string &filename) {
  std::ofstream csvFile(filename);

  if (!csvFile.is_open()) {
    std::cerr << "Failed to open the CSV file for writing." << std::endl;
    return;
  }

  // Write a header row with column names
  csvFile << "X,Y,SquareType,Data\n";

  // Write size
  csvFile << city.getCitySize() << '\n';

  // Loop through the city and write each square's data
  for (size_t x = 0; x < city.getCitySize(); x++) {
    for (size_t y = 0; y < city.getCitySize(); y++) {
      Square *square = city.getSquare(Position(x, y));

      // Write the position and square type
      csvFile << x << "," << y << ",";
      if (square->isBuilding()) {
        Building *building = static_cast<Building *>(square);
        if (building->buildingType() == BuildingType::Residential) {
          csvFile << "Residential," + std::to_string(building->getCapacity()) + "\n";
        } else if (building->buildingType() == BuildingType::Industrial) {
          csvFile << "Industrial,-\n"; // placeholder
        } else if (building->buildingType() == BuildingType::Commercial) {
          csvFile << "Commercial,-\n"; // placeholder
        }
      } else if (square->isIntersection()) {
        Intersection *intersection = static_cast<Intersection *>(square);
        csvFile << "Intersection," + std::to_string(intersection->getType()) + "\n";
      } else if (square->isRoad()) {
        Road *road = static_cast<Road *>(square);
        csvFile << "Road," + std::to_string(road->getSpeed()) + "\n";
      } else {
        csvFile << "Empty,-\n";
      }
    }
  }

  csvFile.close();
  std::cout << "City data has been written to " << filename << std::endl;
}

void GUI::drawHistogram(std::vector<double> data, std::string yLabel, int type) {
  histogramWindow_.clear(sf::Color::White);
  const int barWidth = 30;
  const int spacing = 10;
  const int maxHeight = 605;
  const int offsetX = 30;
  const int offsetY = 650;

  // Draw x-axis
  sf::RectangleShape xAxis(sf::Vector2f(data.size() * (barWidth + spacing), 2));
  xAxis.setPosition(offsetX, offsetY);
  xAxis.setFillColor(sf::Color::Black);
  histogramWindow_.draw(xAxis);

  // Draw y-axis
  sf::RectangleShape yAxis(sf::Vector2f(2, 600));
  yAxis.setPosition(offsetX, offsetY - maxHeight + 5);
  yAxis.setFillColor(sf::Color::Black);
  histogramWindow_.draw(yAxis);

  // Draw x-axis label
  sf::Text xAxisLabel("Hour", font_, 20);
  xAxisLabel.setPosition(offsetX + data.size() * (barWidth + spacing) - 50, offsetY + 20);
  xAxisLabel.setFillColor(sf::Color::Black);

  // Draw y-axis label
  sf::Text yAxisLabel(yLabel, font_, 20);
  yAxisLabel.setPosition(offsetX - 10, 10);
  yAxisLabel.setFillColor(sf::Color::Black);

  // Draw bars and hour labels
  for (size_t i = 0; i < data.size(); i++) {
    sf::RectangleShape bar;
    if (type == 1) { // Road
      bar.setSize(sf::Vector2f(barWidth, -data[i] * 60));
    } else if (type == 2) { // City
      bar.setSize(sf::Vector2f(barWidth, -data[i] * 600));
    } else if (type == 3) { // Intersection
      auto maxElement = std::max_element(data.begin(), data.end());
      int maxValue = 0;
      if (maxElement != data.end()) {
        maxValue = *maxElement;
      }
      bar.setSize(sf::Vector2f(barWidth, -data[i] / maxValue * 600));
    }
    bar.setPosition(offsetX + i * (barWidth + spacing) + 5, offsetY);
    bar.setFillColor(sf::Color::Blue);
    histogramWindow_.draw(bar);

    // Draw hour label
    sf::Text hourLabel(std::to_string(i), font_, 15);
    hourLabel.setPosition(offsetX + i * (barWidth + spacing) + barWidth / 2 - 4, offsetY + 5);
    hourLabel.setFillColor(sf::Color::Black);
    histogramWindow_.draw(hourLabel);
  }

  // Draw notches on y-axis
  for (int notch = 60; notch < maxHeight; notch += 60) {
    sf::RectangleShape yNotch(sf::Vector2f(5, 2));
    yNotch.setPosition(offsetX - 5, offsetY - notch);
    yNotch.setFillColor(sf::Color::Black);
    histogramWindow_.draw(yNotch);

    // Draw notch label
    sf::Text notchLabel;
    if (type == 1) {
      notchLabel.setString(std::to_string(notch / 60));
    } else if (type == 2) {
      notchLabel.setString(std::to_string(notch / 6));
    } else if (type == 3) {
      auto maxElement = std::max_element(data.begin(), data.end());
      int maxValue = 0;
      if (maxElement != data.end()) {
        maxValue = *maxElement;
      }
      notchLabel.setString(std::to_string(maxValue / 10 * (notch / 60)));
    }
    notchLabel.setFont(font_);
    notchLabel.setCharacterSize(15);
    notchLabel.setPosition(offsetX - 29, offsetY - notch - 8);
    notchLabel.setFillColor(sf::Color::Black);
    histogramWindow_.draw(notchLabel);
  }

  histogramWindow_.draw(yAxisLabel);
  histogramWindow_.draw(xAxisLabel);

// Display the changes
#ifdef __APPLE__
  // Code specific to macOS
  histogramWindow_.display();
  histogramWindow_.setVisible(true);
#else
  // Code for other operating systems
  histogramWindow_.setVisible(true);
  histogramWindow_.display();
#endif
}

GUI::GUI(City &city, int size)
    : isAddBuildingButtonPressed_(false), isPopUp_(false), paused_(true), cityChanged_(false),
      size_(size), window_(sf::RenderWindow(sf::VideoMode(1000, 800), "Traffic Simulator")),
      city_(city), currentTool_(Tool::None), userInput_(""), isInputBoxVisible_(false),
      residentCount_(100), roadPopUp_(false), isAddRoadButtonPressed_(false),
      histogramWindow_(sf::RenderWindow(sf::VideoMode(1100, 800), "Histogram Window")),
      isHeatMap_(false) {
  cellSize_ = static_cast<float>(window_.getSize().y) / size_;
}

void GUI::init() {
  histogramWindow_.setVisible(false);
  window_.setFramerateLimit(60);
  histogramWindow_.setFramerateLimit(60);

  if (!font_.loadFromFile("../libs/Arial.ttf")) {
    std::cerr << "Failed to load font." << std::endl;
  }
  sf::RectangleShape menu(sf::Vector2f(200, window_.getSize().y));
  menu.setFillColor(sf::Color(200, 200, 200));

  sf::RectangleShape separator1(sf::Vector2f(200, 1));
  separator1.setFillColor(sf::Color::Black);
  separator1.setPosition(0, 40);

  sf::RectangleShape separator2(sf::Vector2f(200, 1));
  separator2.setFillColor(sf::Color::Black);
  separator2.setPosition(0, 90);

  sf::RectangleShape separator3(sf::Vector2f(200, 1));
  separator3.setFillColor(sf::Color::Black);
  separator3.setPosition(0, 140);

  sf::RectangleShape separator4(sf::Vector2f(200, 1));
  separator4.setFillColor(sf::Color::Black);
  separator4.setPosition(0, 190);

  sf::RectangleShape separator5(sf::Vector2f(200, 1));
  separator5.setFillColor(sf::Color::Black);
  separator5.setPosition(0, 240);

  sf::RectangleShape separator6(sf::Vector2f(200, 1));
  separator6.setFillColor(sf::Color::Black);
  separator6.setPosition(0, 290);

  staticRectangles_.push_back(menu);
  staticRectangles_.push_back(separator1);
  staticRectangles_.push_back(separator2);
  staticRectangles_.push_back(separator3);
  staticRectangles_.push_back(separator4);
  staticRectangles_.push_back(separator5);
  staticRectangles_.push_back(separator6);

  deleteText_.setFont(font_);
  addBuildingText_.setFont(font_);
  addRoadText_.setFont(font_);
  heatMapText_.setFont(font_);
  pauseText_.setFont(font_);
  fastForward_.setFont(font_);

  deleteText_.setCharacterSize(20);
  addBuildingText_.setCharacterSize(20);
  addRoadText_.setCharacterSize(20);
  heatMapText_.setCharacterSize(20);
  pauseText_.setCharacterSize(20);
  fastForward_.setCharacterSize(20);

  deleteText_.setString("Delete");
  addBuildingText_.setString("Add Building");
  addRoadText_.setString("Add Road");
  heatMapText_.setString("Show heatmap");
  pauseText_.setString("Pause");
  fastForward_.setString("Fast forward");

  deleteText_.setPosition(20, 20 - 10);
  addBuildingText_.setPosition(20, 60 - 5);
  addRoadText_.setPosition(20, 100 + 5);
  heatMapText_.setPosition(20, 140 + 15);
  pauseText_.setPosition(20, 200 + 5);
  fastForward_.setPosition(20, 250 + 5);

  sf::RectangleShape dropdown;
  dropdown.setFillColor(sf::Color(200, 200, 200));
  dropdown_ = dropdown;

  residentialText_.setFont(font_);
  industrialText_.setFont(font_);
  commercialText_.setFont(font_);

  residentialText_.setCharacterSize(20);
  industrialText_.setCharacterSize(20);
  commercialText_.setCharacterSize(20);

  residentialText_.setString("Residential");
  industrialText_.setString("Industrial");
  commercialText_.setString("Commercial");

  residentialText_.setPosition(40, 90);
  industrialText_.setPosition(40, 130);
  commercialText_.setPosition(40, 170);

  road30Text_.setFont(font_);
  road40Text_.setFont(font_);
  road60Text_.setFont(font_);
  intersectionLightsText_.setFont(font_);
  intersectionNoLightsText_.setFont(font_);

  road30Text_.setCharacterSize(20);
  road40Text_.setCharacterSize(20);
  road60Text_.setCharacterSize(20);
  intersectionLightsText_.setCharacterSize(15);
  intersectionNoLightsText_.setCharacterSize(15);

  road30Text_.setString("30km/h Road");
  road40Text_.setString("40km/h Road");
  road60Text_.setString("60km/h Road");
  intersectionLightsText_.setString("Intersection with lights");
  intersectionNoLightsText_.setString("Intersection with no lights");

  road30Text_.setPosition(20, 140);
  road40Text_.setPosition(20, 180);
  road60Text_.setPosition(20, 220);
  intersectionLightsText_.setPosition(20, 260);
  intersectionNoLightsText_.setPosition(20, 300);

  sf::RectangleShape popUp;
  popUp.setSize(sf::Vector2f(200, 200));
  popUp.setFillColor(sf::Color(200, 200, 200));
  popUp.setOutlineColor(sf::Color::Black);
  popUp_ = popUp;

  sf::Text popUpText;
  popUpText.setFont(font_);
  popUpText.setCharacterSize(14);
  popUpText_ = popUpText;

  sf::RectangleShape inputBox(sf::Vector2f(100, 40));
  inputBox.setFillColor(sf::Color::White);
  inputBox.setOutlineColor(sf::Color::Black);
  inputBox.setOutlineThickness(2);
  inputBox.setPosition(10, 5);
  inputBox_ = inputBox;

  sf::Text inputText;
  inputText.setFont(font_);
  inputText.setCharacterSize(20);
  inputText.setFillColor(sf::Color::Black);
  inputText.setPosition(10, 10);
  inputText_ = inputText;

  time_.setFont(font_);
  time_.setCharacterSize(20);
  time_.setPosition(20, 300);

  histButton_.setFillColor(sf::Color::Black);
  histButton_.setSize(sf::Vector2f(50, 30));
  histText_.setFont(font_);
  histText_.setCharacterSize(10);
  histText_.setString("Show \nhistogram");

  analysisText_.setFillColor(sf::Color::Black);
  analysisText_.setFont(font_);
  analysisText_.setCharacterSize(15);
  analysisText_.setFillColor(sf::Color::Black);
  analysisText_.setPosition(5, 620);

  cityHistButton_.setSize(sf::Vector2f(198, 40));
  cityHistButton_.setFillColor(sf::Color(200, 200, 200));
  cityHistButton_.setOutlineColor(sf::Color::Black);
  cityHistButton_.setOutlineThickness(2);
  cityHistButton_.setPosition(0, 760);

  cityHistText_.setFont(font_);
  cityHistText_.setFillColor(sf::Color::White);
  cityHistText_.setCharacterSize(20);
  cityHistText_.setPosition(20, 760);
  cityHistText_.setString("Show histogram");

  exportDataButton_.setSize(sf::Vector2f(198, 40));
  exportDataButton_.setFillColor(sf::Color(200, 200, 200));
  exportDataButton_.setOutlineColor(sf::Color::Black);
  exportDataButton_.setOutlineThickness(2);
  exportDataButton_.setPosition(0, 720);

  exportDataText_.setFont(font_);
  exportDataText_.setFillColor(sf::Color::White);
  exportDataText_.setCharacterSize(20);
  exportDataText_.setPosition(20, 730);
  exportDataText_.setString("Export data");

  backdrop_.setSize(sf::Vector2f(1020, 800));
  backdrop_.setFillColor(sf::Color::White);
  backdrop_.setPosition(0, 0);
}

void GUI::handleEvent() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window_.close();
    } else if (event.type == sf::Event::Resized) {
      // Handle window resize
      /*
      int newWindowSize = std::min(event.size.width, event.size.height);
      window_.setSize(sf::Vector2u(newWindowSize, newWindowSize));
      menu_.setSize(sf::Vector2f(200, newWindowSize));
      cellSize = static_cast<float>(newWindowSize) / size;
      */
    } else if ((event.type == sf::Event::KeyPressed) && !isInputBoxVisible_) {
      if (event.key.code == sf::Keyboard::I && paused_) {
        readCSV("City.csv");
        size_ = city_.getCitySize();
        cellSize_ = static_cast<float>(window_.getSize().y) / size_;
      } else if (event.key.code == sf::Keyboard::E) {
        writeCSV(city_, "City.csv");
      } else if (event.key.code == sf::Keyboard::A) {
        isInputBoxVisible_ = true;
        paused_ = true;
        userInput_.clear();
      }
    } else if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        // Get the mouse position
        sf::Vector2i mousePos = sf::Mouse::getPosition(window_);

        // Calculate the cell that was clicked
        int cellX = static_cast<int>((mousePos.x - 200) / cellSize_);
        int cellY = static_cast<int>(mousePos.y / cellSize_);

        sf::FloatRect buttonBounds = histButton_.getGlobalBounds();
        sf::FloatRect cityButtonBounds = cityHistButton_.getGlobalBounds();
        sf::FloatRect exportDataBounds = exportDataButton_.getGlobalBounds();
        // Handle menu interactions
        if (mousePos.x < 200) {
          if (isAddBuildingButtonPressed_ && mousePos.y > 90) {
            if (mousePos.y < 130) {
              currentTool_ = Tool::Residential;
              isAddBuildingButtonPressed_ = false;
            } else if (mousePos.y < 170) {
              currentTool_ = Tool::Industrial;
              isAddBuildingButtonPressed_ = false;
            } else if (mousePos.y < 210) {
              currentTool_ = Tool::Commercial;
              isAddBuildingButtonPressed_ = false;
            }
          } else if (isAddRoadButtonPressed_ && mousePos.y > 140) {
            if (mousePos.y < 180) {
              currentTool_ = Tool::Road30;
              isAddRoadButtonPressed_ = false;
            } else if (mousePos.y < 220) {
              currentTool_ = Tool::Road40;
              isAddRoadButtonPressed_ = false;
            } else if (mousePos.y < 260) {
              currentTool_ = Tool::Road60;
              isAddRoadButtonPressed_ = false;
            } else if (mousePos.y < 300) {
              currentTool_ = Tool::IntersectionLights;
              isAddRoadButtonPressed_ = false;
            } else if (mousePos.y < 340) {
              currentTool_ = Tool::IntersectionNoLights;
              isAddRoadButtonPressed_ = false;
            }
          } else if (mousePos.y < 40) {
            // Delete button
            if (currentTool_ == Tool::Delete) {
              currentTool_ = Tool::None;
            } else {
              currentTool_ = Tool::Delete;
            }
          } else if (mousePos.y < 90) {
            // Add Building button
            isAddBuildingButtonPressed_ = !isAddBuildingButtonPressed_;
            isAddRoadButtonPressed_ = false;
            dropdown_.setSize(sf::Vector2f(200, 120));
            dropdown_.setPosition(0, 91);
            currentTool_ = Tool::None;
          } else if (mousePos.y < 140) {
            // Add Road button
            isAddRoadButtonPressed_ = !isAddRoadButtonPressed_;
            isAddBuildingButtonPressed_ = false;
            dropdown_.setSize(sf::Vector2f(200, 200));
            dropdown_.setPosition(0, 141);
            currentTool_ = Tool::None;
          } else if (mousePos.y < 190) {
            isHeatMap_ = !isHeatMap_;
          } else if (mousePos.y < 240) {
            paused_ = !paused_;
          } else if (mousePos.y < 290) {
            if (currentTool_ == Tool::FastForward10x) {
              currentTool_ = Tool::None;
            } else if (currentTool_ == Tool::FastForward) {
              currentTool_ = Tool::FastForward10x;
            } else {
              currentTool_ = Tool::FastForward;
            }
          } else if (cityButtonBounds.contains(mousePos.x, mousePos.y)) {
            currentStats_ = city_.getStats();
            currentHistType_ = 2;
            currentHistLabel_ = "Traffic percentage (%)";
            drawHistogram(currentStats_, currentHistLabel_, currentHistType_);
          } else if (exportDataBounds.contains(mousePos.x, mousePos.y)) {
            city_.exportStats("TrafficData.csv");
          }
        } else {
          // Handle grid interactions
          if (cellX >= 0 && cellX < size_ && cellY >= 0 && cellY < size_) {
            Position pos(cellX, cellY);
            if (currentTool_ != Tool::None) {
              if (paused_) {
                if (currentTool_ == Tool::Residential) {
                  Residential *square = new Residential(pos, city_, residentCount_);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::Industrial) {
                  Industrial *square = new Industrial(pos, city_, 0);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::Commercial) {
                  Commercial *square = new Commercial(pos, city_, 0);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::Delete) {
                  Square *square = new Square(pos, city_);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::Road30) {
                  Square *square = new Road(pos, city_, 2);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::Road40) {
                  Square *square = new Road(pos, city_, 1);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::Road60) {
                  Square *square = new Road(pos, city_, 0);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::IntersectionLights) {
                  std::vector<Direction> empty = {Direction::None};
                  Square *square = new Intersection(pos, city_, empty, 1);
                  city_.add(pos, square);
                  cityChanged_ = true;
                } else if (currentTool_ == Tool::IntersectionNoLights) {
                  std::vector<Direction> empty = {Direction::None};
                  Square *square = new Intersection(pos, city_, empty, 0);
                  city_.add(pos, square);
                  cityChanged_ = true;
                }
              }
            } else if (roadPopUp_ && buttonBounds.contains(mousePos.x, mousePos.y)) {
              if (currentHistType_ == 1) {
                currentHistLabel_ = "Average car amount";
              } else {
                currentHistLabel_ = "Total cars";
              }
              drawHistogram(currentStats_, currentHistLabel_, currentHistType_);
            } else {
              Square *square = city_.getSquare(pos);
              if (mousePos.x > 800 && mousePos.y > 600) {
                popUp_.setPosition(mousePos.x + 5 - 200, mousePos.y + 5 - 200);
                popUpText_.setPosition(mousePos.x + 10 - 200, mousePos.y + 5 - 200);
              } else if (mousePos.y > 600) {
                popUp_.setPosition(mousePos.x + 5, mousePos.y + 5 - 200);
                popUpText_.setPosition(mousePos.x + 10, mousePos.y + 5 - 200);
              } else if (mousePos.x > 800) {
                popUp_.setPosition(mousePos.x + 5 - 200, mousePos.y + 5);
                popUpText_.setPosition(mousePos.x + 10 - 200, mousePos.y + 5);
              } else {
                popUp_.setPosition(mousePos.x + 5, mousePos.y + 5);
                popUpText_.setPosition(mousePos.x + 10, mousePos.y + 5);
              }
              if (square->isIntersection()) {
                Intersection *intersection = static_cast<Intersection *>(square);
                int carAmount = intersection->getCarCount();
                int pedAmount = intersection->getPedCount();
                int type = intersection->getType();
                if (type == 0) {
                  std::string desc = "This is an intersection\nwithout traffic lights\nwith " +
                                     std::to_string(carAmount) + " cars and\n" +
                                     std::to_string(pedAmount) + " pedestrians on it.";
                  popUpText_.setString(desc);
                } else {
                  std::string desc = "This is an intersection\nwith traffic lights\nwith " +
                                     std::to_string(carAmount) + " cars and\n" +
                                     std::to_string(pedAmount) + " pedestrians on it.";
                  popUpText_.setString(desc);
                }
                if (mousePos.x > 800 && mousePos.y > 600) {
                  histText_.setPosition(mousePos.x + 160 - 200, mousePos.y + 5 - 200);
                  histButton_.setPosition(mousePos.x + 155 - 200, mousePos.y + 5 - 200);
                } else if (mousePos.y > 600) {
                  histText_.setPosition(mousePos.x + 160, mousePos.y + 5 - 200);
                  histButton_.setPosition(mousePos.x + 155, mousePos.y + 5 - 200);
                } else if (mousePos.x > 800) {
                  histText_.setPosition(mousePos.x + 160 - 200, mousePos.y + 5);
                  histButton_.setPosition(mousePos.x + 155 - 200, mousePos.y + 5);
                } else {
                  histText_.setPosition(mousePos.x + 160, mousePos.y + 5);
                  histButton_.setPosition(mousePos.x + 155, mousePos.y + 5);
                }
                currentStats_ = intersection->getStats();
                currentHistType_ = 3;
                roadPopUp_ = true;
              } else if (square->isRoad()) {
                Road *road = static_cast<Road *>(square);
                int pedAmount = road->getPedCount();
                int carAmount = road->getCarCount();
                int speed = 0;
                switch (road->getSpeed()) {
                case 0:
                  speed = 60;
                  break;
                case 1:
                  speed = 40;
                  break;
                case 2:
                  speed = 30;
                  break;
                default:
                  break;
                }
                std::string desc = "This is a road\nwith a speed limit of\n" +
                                   std::to_string(speed) + " km/h and with " +
                                   std::to_string(carAmount) + " cars and\n" +
                                   std::to_string(pedAmount) + " pedestrians on it.";
                popUpText_.setString(desc);
                // Give data and a button to open a histogram window
                if (mousePos.x > 800 && mousePos.y > 600) {
                  histText_.setPosition(mousePos.x + 160 - 200, mousePos.y + 5 - 200);
                  histButton_.setPosition(mousePos.x + 155 - 200, mousePos.y + 5 - 200);
                } else if (mousePos.y > 600) {
                  histText_.setPosition(mousePos.x + 160, mousePos.y + 5 - 200);
                  histButton_.setPosition(mousePos.x + 155, mousePos.y + 5 - 200);
                } else if (mousePos.x > 800) {
                  histText_.setPosition(mousePos.x + 160 - 200, mousePos.y + 5);
                  histButton_.setPosition(mousePos.x + 155 - 200, mousePos.y + 5);
                } else {
                  histText_.setPosition(mousePos.x + 160, mousePos.y + 5);
                  histButton_.setPosition(mousePos.x + 155, mousePos.y + 5);
                }
                currentStats_ = road->getStats();
                currentHistType_ = 1;
                roadPopUp_ = true;
              } else if (square->isBuilding()) {
                Building *building = static_cast<Building *>(square);
                int residentAmount = building->getResidents();
                BuildingType type = building->buildingType();
                roadPopUp_ = false;
                if (type == BuildingType::Commercial) {
                  std::string desc = "This is a commercial building\nwith " +
                                     std::to_string(residentAmount) + " occupants.";
                  popUpText_.setString(desc);
                } else if (type == BuildingType::Store) {
                  std::string desc = "This is a store with " + std::to_string(residentAmount) +
                                     " people currently shopping.";
                  popUpText_.setString(desc);
                } else if (type == BuildingType::Gym) {
                  std::string desc = "This is a gym with " + std::to_string(residentAmount) +
                                     " people currently working out.";
                  popUpText_.setString(desc);
                } else if (type == BuildingType::Restaurant) {
                  std::string desc = "This is a restaurant with " + std::to_string(residentAmount) +
                                     " people currently dining.";
                  popUpText_.setString(desc);
                } else if (type == BuildingType::Industrial) {
                  std::string desc = "This is an industrial\nbuilding with " +
                                     std::to_string(residentAmount) + " people\ncurrently working.";
                  popUpText_.setString(desc);
                } else if (type == BuildingType::Residential) {
                  std::string desc = "This is a residential\nbuilding with " +
                                     std::to_string(building->getCapacity()) + " residents,\n" +
                                     std::to_string(residentAmount) +
                                     " of which are home.\n(You might need to "
                                     "unpause\nto update this number)";
                  popUpText_.setString(desc);
                }
              } else if (square->isEmpty()) {
                roadPopUp_ = false;
                popUpText_.setString("This is an empty square");
              }
              isPopUp_ = !isPopUp_;
            }
          }
        }
      }
    } else if (event.type == sf::Event::TextEntered && isInputBoxVisible_) {
      if (event.text.unicode < 128) {
        if (event.text.unicode == 10 || event.text.unicode == 13) {
          // Handle Enter key
          std::cout << "User input: " << userInput_ << std::endl;
          if (std::stoi(userInput_) > 5000) {
            std::cout << "Input for resident count too large!" << std::endl;
          } else {
            residentCount_ = std::stoi(userInput_);
          }
          userInput_.clear();
          isInputBoxVisible_ = false;
        } else if (event.text.unicode == 8 && !userInput_.empty()) {
          // Handle backspace
          userInput_.pop_back();
        } else if (std::isdigit(event.text.unicode)) {
          // Append valid characters to the input
          userInput_ += static_cast<char>(event.text.unicode);
        }
      }
    }
  }
  sf::Event histEvent;
  if (histogramWindow_.isOpen()) {
    while (histogramWindow_.pollEvent(histEvent)) {
      if (histEvent.type == sf::Event::Closed) {
        histogramWindow_.close();
        histIsOpen_ = false;
        histogramWindow_.create(sf::VideoMode(1100, 800), "Histogram Window");
        histogramWindow_.setVisible(false);
      }
    }
  }
}

void GUI::drawAndDisplay() {
  // Update button colors here
  deleteText_.setFillColor((currentTool_ == Tool::Delete) ? pressedButtonColor_
                                                          : defaultButtonColor_);
  heatMapText_.setFillColor((isHeatMap_) ? pressedButtonColor_ : defaultButtonColor_);
  addBuildingText_.setFillColor(
      (isAddBuildingButtonPressed_ || (currentTool_ == Tool::Residential) ||
       (currentTool_ == Tool::Industrial) || (currentTool_ == Tool::Commercial))
          ? pressedButtonColor_
          : defaultButtonColor_);
  addRoadText_.setFillColor(((currentTool_ == Tool::AddRoad) || (currentTool_ == Tool::Road30) ||
                             (currentTool_ == Tool::Road40) || (currentTool_ == Tool::Road60) ||
                             (currentTool_ == Tool::IntersectionLights) ||
                             (currentTool_ == Tool::IntersectionNoLights))
                                ? pressedButtonColor_
                                : defaultButtonColor_);
  pauseText_.setFillColor(paused_ ? pressedButtonColor_ : defaultButtonColor_);
  if (currentTool_ == Tool::FastForward10x) {
    fastForward_.setFillColor(sf::Color::Red);
  } else if (currentTool_ == Tool::FastForward) {
    fastForward_.setFillColor(pressedButtonColor_);
  } else {
    fastForward_.setFillColor(defaultButtonColor_);
  }
  window_.clear();
  window_.draw(backdrop_);
  // Draw the grid with highlighting
  for (size_t x = 0; x < size_; x++) {
    for (size_t y = 0; y < size_; y++) {
      sf::RectangleShape cell(sf::Vector2f(cellSize_, cellSize_));
      sf::RectangleShape lane1;
      sf::RectangleShape lane2;
      sf::Text carAmount;
      cell.setPosition(x * cellSize_ + 200, y * cellSize_);
      Position pos(x, y);
      if (city_.getSquare(pos)->isBuilding()) {
        Building *building = static_cast<Building *>(city_.getSquare(pos));
        if (isHeatMap_) {
          cell.setFillColor(sf::Color::Black);
        } else if (building->buildingType() == BuildingType::Residential) {
          cell.setFillColor(sf::Color::Green);
        } else if (building->buildingType() == BuildingType::Industrial) {
          cell.setFillColor(sf::Color::Red);
        } else if (building->buildingType() == BuildingType::Commercial) {
          cell.setFillColor(sf::Color::Blue);
        } else {
          cell.setFillColor(sf::Color::White);
        }
      } else if (city_.getSquare(pos)->isIntersection()) {
        Intersection *intersection = static_cast<Intersection *>(city_.getSquare(pos));
        if (isHeatMap_) {
          cell.setPosition(x * cellSize_ + 200, y * cellSize_);
          Position pos(x, y);
          int carCount = intersection->getCarCount();
          int maxCount = intersection->maxCarAmount();
          sf::Color color;
          if (carCount >= 0 && carCount <= maxCount / 2) {
            color = sf::Color(static_cast<int>(carCount / (maxCount / 2.0) * 255), 255,
                              0); // Green to Yellow
          } else {
            color = sf::Color(255, static_cast<int>((maxCount - carCount) / (maxCount / 2.0) * 255),
                              0); // Yellow to Red
          }
          cell.setFillColor(color);
        } else {
          cell.setFillColor(sf::Color::Black);
          carAmount.setCharacterSize(10 * 1.0 / (0.02 * size_));
          carAmount.setFont(font_);
          carAmount.setFillColor(sf::Color::White);
          carAmount.setString(std::to_string(intersection->getCarCount()));
          if(intersection->getCarCount() > 9){
            carAmount.setPosition(x * cellSize_ + 200 + cellSize_ / 10.0, y * cellSize_);
          } else{
            carAmount.setPosition(x * cellSize_ + 200 + cellSize_ / 4.0, y * cellSize_);
          }
          carAmount.setStyle(sf::Text::Bold);
        }
      } else if (city_.getSquare(pos)->isRoad()) {
        Road *road = static_cast<Road *>(city_.getSquare(pos));
        if (isHeatMap_) {
          if (road->isNS()) {
            lane1.setSize(sf::Vector2f(cellSize_ / 2, cellSize_));
            lane2.setSize(sf::Vector2f(cellSize_ / 2, cellSize_));
            lane1.setPosition(x * cellSize_ + cellSize_ / 2 + 200, y * cellSize_);
            lane2.setPosition(x * cellSize_ + 200, y * cellSize_);
          } else if (road->isEW()) {
            lane2.setSize(sf::Vector2f(cellSize_, cellSize_ / 2));
            lane1.setSize(sf::Vector2f(cellSize_, cellSize_ / 2));
            lane2.setPosition(x * cellSize_ + 200, y * cellSize_);
            lane1.setPosition(x * cellSize_ + 200, y * cellSize_ + cellSize_ / 2);
          }

          int lane1Count = road->getNECarCount();
          sf::Color color1;
          if (lane1Count >= 0 && lane1Count <= 2) {
            color1 = sf::Color(static_cast<int>(lane1Count / 2.0f * 255), 255,
                               0); // Green to Yellow
          } else {
            color1 = sf::Color(255, static_cast<int>((5 - lane1Count) / 3.0f * 255),
                               0); // Yellow to Red
          }
          lane1.setFillColor(color1);
          int lane2Count = road->getSWCarCount();
          sf::Color color2;
          if (lane2Count >= 0 && lane2Count <= 2) {
            color2 = sf::Color(static_cast<int>(lane2Count / 2.0f * 255), 255,
                               0); // Green to Yellow
          } else {
            color2 = sf::Color(255, static_cast<int>((5 - lane2Count) / 3.0f * 255),
                               0); // Yellow to Red
          }
          lane2.setFillColor(color2);
          lane1.setOutlineThickness(1.0f);
          lane2.setOutlineThickness(1.0f);
          lane1.setOutlineColor(sf::Color::Black);
          lane2.setOutlineColor(sf::Color::Black);
        } else {
          cell.setFillColor(sf::Color::Black);
          carAmount.setCharacterSize(10 * 1.0 / (0.02 * size_));
          carAmount.setFont(font_);
          carAmount.setFillColor(sf::Color::White);
          carAmount.setString(std::to_string(road->getCarCount()));
          if(road->getCarCount() > 9){
            carAmount.setPosition(x * cellSize_ + 200 + cellSize_ / 10.0, y * cellSize_);
          } else{
            carAmount.setPosition(x * cellSize_ + 200 + cellSize_ / 4.0, y * cellSize_);
          }
          carAmount.setStyle(sf::Text::Bold);
        }
      } else {
        cell.setFillColor(sf::Color::White);
      }
      cell.setOutlineColor(sf::Color::Black);
      cell.setOutlineThickness(1.0f);
      window_.draw(cell);
      window_.draw(lane1);
      window_.draw(lane2);
      window_.draw(carAmount);
    }
  }
  for (const auto &rect : staticRectangles_) {
    window_.draw(rect);
  }
  int hours = static_cast<int>(city_.getTime());
  int minutes = static_cast<int>((city_.getTime() - hours) * 60);
  std::ostringstream timess;
  timess << "Time: " << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
         << std::setfill('0') << minutes;
  time_.setString(timess.str());
  window_.draw(deleteText_);
  window_.draw(addBuildingText_);
  window_.draw(addRoadText_);
  window_.draw(heatMapText_);
  window_.draw(pauseText_);
  window_.draw(time_);
  window_.draw(fastForward_);
  if (isInputBoxVisible_) {
    inputText_.setString(userInput_);
    window_.draw(inputBox_);
    window_.draw(inputText_);
  }
  if (isAddBuildingButtonPressed_) {
    window_.draw(dropdown_);
    window_.draw(residentialText_);
    window_.draw(industrialText_);
    window_.draw(commercialText_);
  }
  if (isAddRoadButtonPressed_) {
    window_.draw(dropdown_);
    window_.draw(road30Text_);
    window_.draw(road40Text_);
    window_.draw(road60Text_);
    window_.draw(intersectionLightsText_);
    window_.draw(intersectionNoLightsText_);
  }
  if (isPopUp_) {
    window_.draw(popUp_);
    window_.draw(popUpText_);
    if (roadPopUp_) {
      window_.draw(histButton_);
      window_.draw(histText_);
    }
  }
  std::ostringstream dataStream;
  dataStream << "Residents: " << city_.getResidentAmount()
             << "\nCars on roads: " << city_.carsOnRoads() << "\nCurrent traffic: " << std::fixed
             << std::setprecision(1) << 100 * city_.currentRoadTrafficPercentage() << "%"
             << "\nAverage traffic: " << std::fixed << std::setprecision(1)
             << 100 * city_.averageRoadTrafficPercentage() << "%"
             << "\nPedestrians on roads: " << city_.getPedCount();

  std::string dataText = dataStream.str();
  analysisText_.setString(dataText);
  window_.draw(analysisText_);

  window_.draw(cityHistButton_);
  window_.draw(cityHistText_);
  window_.draw(exportDataButton_);
  window_.draw(exportDataText_);

  window_.display();
  if (histIsOpen_) {
    drawHistogram(currentStats_, currentHistLabel_, currentHistType_);
  }
}