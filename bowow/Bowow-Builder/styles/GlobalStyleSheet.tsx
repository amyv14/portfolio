import { StyleSheet } from "react-native";

export const GlobalColors = {
  yale_blue: "#002F6A",
  fun_orange: "#FF6B00",
  white: "#FFFFFF",
};

const GlobalStyles = StyleSheet.create({
  title: {
    fontSize: 50,
    color: "white",
    fontFamily: "Georgia",
    textAlign: "left",
  },
  header: {
    fontSize: 24,
    fontWeight: "bold",
    marginBottom: 20,
    color: "white",
    fontFamily: "Georgia",
  },
  subHeader: {
    lineHeight: 24,
    color: "white",
    marginTop: 5,
    fontSize: 17,
  },
  link: {
    marginTop: 20,
    color: "white",
    fontSize: 33,
  },
  centeredContainer: {
    justifyContent: "center",
    padding: 30,
  },
  container: {
    padding: 20,
    marginBottom: 20,
  },
  button: {
    backgroundColor: "#FF6B00",
    paddingVertical: 15,
    borderRadius: 10,
    alignItems: "center",
    marginTop: 20,
  },
  buttonText: {
    color: "white",
    fontSize: 18,
    fontWeight: "bold",
    letterSpacing: 1.5,
  },
  background: {
    flex: 1,
    width: "100%",
    height: "100%",
  },
  image: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
  textBox: {
    marginBottom: 20,
    alignItems: "center",
  },
  input: {
    height: 46,
    fontSize: 13,
    paddingLeft: 10,
    borderWidth: 1,
    borderRadius: 10,
    marginTop: 20,
    width: "100%",
    color: "#000080",
    borderColor: "#BEDCF9",
    backgroundColor: "#BEDCF9",
  },
  reminder: {
    fontSize: 14,
    color: '#002F6A',
  },
  divider: {
      borderBottomColor: "silver",
      borderBottomWidth: 2,
  }
});

export default GlobalStyles;
