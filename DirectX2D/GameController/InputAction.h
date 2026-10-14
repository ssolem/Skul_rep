#pragma once

class InputAction
{
public:
    enum class InputType
    {
        NONE, DOWN, UP, PRESS
    };

    InputAction(UINT key, InputType type, void* value = nullptr) :
        key(key), type(type), value(value) {}
    ~InputAction() = default;

    bool InputApplied();

    UINT GetKey() { return key; }
    void SetKey(UINT key) { this->key = key; }

    void* GetValue() { return value; }
    void SetValue(void* value) { this->value = value; }

    InputType GetInputType() { return type; }
    void SetInputType(InputType type) { this->type = type; }

private:
	UINT key;
    InputType type;
    void* value;    // <- 입력에 중속된 값을 그저 읽을 뿐. (착불 갑시다)

};

