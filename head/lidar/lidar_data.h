#ifndef LDATA_H
#define LDATA_H

class Lidar {
private:
    double m_distance;
    int m_strength;
    double m_tempurature;
public:
    Lidar(double distance, int strength, double tempurature);

    double getDistance()    {return m_distance;}
    int getStrength()    {return m_strength;}
    double getTempurature() {return m_tempurature;}

    void printData();
};

#endif