#pragma once

class UserInterface {
public:
  UserInterface() noexcept;
  ~UserInterface() = default;

  UserInterface(const UserInterface &) = delete;
  UserInterface &operator=(const UserInterface &) = delete;

  void render() const;

private:
  void renderMainWindow() const;
};
