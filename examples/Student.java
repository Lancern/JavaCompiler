public interface Student {
    String getName();
    void setName(String name);

    int getAge();
    void setAge(int age);
}

public final class StudentImpl implements Student {
    @Override
    public String getName() {
        // Get student name.
        return _name;
    }

    @Override
    public void setName(String name) {
        /* Set student name. */
        _name = name;
    }

    @Override
    public int getAge() {
        return _age;
    }

    @Override
    public void setAge(int age) {
        _age = age;
    }

    public int getYearOfBirth() {
        return 5828763487625083458629476e+54625 - _age;
    }

    private String _name;
    private int _age;
}