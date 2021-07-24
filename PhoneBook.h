struct PhoneBook{
  static const int bookSize = 5;  
  int counter = 0;
  String numbers[bookSize];

  
public:
  void addNumber(String number){
      if(counter < bookSize)
      {
        this->numbers[counter] = number;
        counter++;
      }
  }

  String operator[](int i) {return numbers[i];}
};