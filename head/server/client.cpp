class Client {
private:
    float altitude;
    int identifier;
    

public:
    Client();
    
    float getAltitude();
    int getIdent();
    
    void setAltitude(float inputAltitude);
    void setIdent(int newIdent);

};

Client::Client() {
}

float Client::getAltitude() {
    return altitude;
}

int Client::getIdent() {
    return identifier;
}

void Client::setAltitude(float inputAltitude) {
    altitude = inputAltitude;
}

void Client::setIdent( int newIdent) {
    identifier = newIdent;
}