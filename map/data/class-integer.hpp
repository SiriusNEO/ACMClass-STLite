class Integer {
private:
public:
    int data;
	Integer(const int &value) : data(value) {}
	Integer(const Integer &other) : data(other.data) {}
	bool operator==(const Integer &t)
	{
		return data == t.data;
	}
};
