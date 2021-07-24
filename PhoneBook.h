struct PhoneBook{
  static const int bookSize = 10;  
  int counter = 0;
  String numbers[bookSize];
 
public:
  bool addNumber(String number){
      if(counter < bookSize)
      {
        this->numbers[counter] = number;
        counter++;
        return true;
      }
      return false;
  }

  String operator[](int i) { return numbers[i]; }

  int size() const { return counter; }
};