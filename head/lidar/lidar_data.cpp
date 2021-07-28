#include "lidar_data.h"
#include <iostream>

Lidar::Lidar(double distance, int strength, double tempurature) {
    m_distance = distance;
    m_strength = strength;
    m_tempurature = tempurature;
}

//For Debug
void Lidar::printData() {
    std::cout << "Distance: " << m_distance << " ; Strength: " << m_strength << " ; Tempurature: " << m_tempurature << std::endl;
} 