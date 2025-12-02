import React, { useState } from "react";
import {
  View,
  Text,
  TouchableOpacity,
  TextInput,
  Alert,
  StyleSheet,
  ImageBackground,
  StatusBar,
  SafeAreaView,
} from "react-native";
import { Link } from "expo-router";
import AsyncStorage from '@react-native-async-storage/async-storage';
import * as Constants from '../constants';

const API_URL = Constants.IP_ADDRESS + 'signup';

export default function SignupScreen() {
  // state hooks for email, username, password, and show password toggle
  const [email, setEmail] = useState('');
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);

  // submit function for handling signup
  const submit = async () => {
    // check if all fields are filled
    if (!email || !username || !password) {
      Alert.alert('Error', 'Please fill in all fields');
      return;
    }
    try {
      // send signup request to the API
      const response = await fetch(API_URL, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ email, username, password }),
      });
      const data = await response.text();
      // handle successful signup response
      if (response.status === 201) {
        Alert.alert('Success', 'User registered successfully!');
        await AsyncStorage.setItem('user', JSON.stringify(data));
      } else {
        // show error if signup fails
        Alert.alert('Error', data || 'Something went wrong');
      }
    } catch (error) {
      console.error(error);
      Alert.alert('Error', 'An error occurred. Please try again.');
    }
  };

  return (
    <ImageBackground
      source={require('../assets/images/gradblue.jpg')}
      style={styles.background}
      resizeMode="cover"
    >
      <StatusBar hidden />
      <SafeAreaView style={styles.safeArea}>
        <View style={styles.formWrapper}>
          <Text style={styles.title}>Sign Up</Text>

          {/* input field for email */}
          <TextInput
            style={styles.input}
            placeholder="Email"
            placeholderTextColor="#DDD"
            value={email}
            onChangeText={setEmail}
            keyboardType="email-address"
            autoCapitalize="none"
          />

          {/* input field for username */}
          <TextInput
            style={styles.input}
            placeholder="Username"
            placeholderTextColor="#DDD"
            value={username}
            onChangeText={setUsername}
            autoCapitalize="none"
          />

          {/* container for password input and show/hide toggle */}
          <View style={styles.passwordContainer}>
            <TextInput
              style={[styles.input, { flex: 1 }]}
              placeholder="Password"
              placeholderTextColor="#DDD"
              value={password}
              onChangeText={setPassword}
              secureTextEntry={!showPassword}
            />
            <TouchableOpacity
              onPress={() => setShowPassword(!showPassword)}
              style={styles.toggleButton}
            >
              {/* button to toggle password visibility */}
              <Text style={styles.toggleText}>
                {showPassword ? 'Hide' : 'Show'}
              </Text>
            </TouchableOpacity>
          </View>

          {/* sign up button */}
          <TouchableOpacity style={styles.button} onPress={submit}>
            <Text style={styles.buttonText}>Sign Up</Text>
          </TouchableOpacity>

          {/* link to login page */}
          <Link href="/login" style={styles.link}>
            <Text style={styles.linkText}>
              Already have an account? Sign in here
            </Text>
          </Link>
        </View>
      </SafeAreaView>
    </ImageBackground>
  );
}

const styles = StyleSheet.create({
  background: {
    flex: 1,
    width: '100%',
    height: '100%',
    justifyContent: 'center',
    alignItems: 'center',
  },
  safeArea: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: 'transparent',
  },
  formWrapper: {
    width: '90%',
    backgroundColor: 'rgba(0, 34, 68, 0.9)',
    padding: 24,
    borderRadius: 20,
    borderWidth: 2,
    borderColor: '#66A3FF',
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 6 },
    shadowOpacity: 0.1,
    shadowRadius: 8,
    elevation: 10,
  },
  title: {
    fontSize: 32,
    color: '#EEFFFF',
    marginBottom: 24,
    fontWeight: '700',
    textAlign: 'center',
  },
  input: {
    height: 50,
    backgroundColor: 'rgba(255, 255, 255, 0.2)',
    borderRadius: 12,
    paddingHorizontal: 16,
    color: '#EEFFFF',
    marginBottom: 16,
    borderWidth: 1,
    borderColor: '#AAD4FF',
    fontSize: 16,
  },
  passwordContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    marginBottom: 16,
  },
  toggleButton: {
    marginLeft: 12,
    padding: 6,
  },
  toggleText: {
    color: '#AAD4FF',
    fontWeight: '600',
    fontSize: 14,
  },
  button: {
    backgroundColor: '#005BBB',
    paddingVertical: 16,
    borderRadius: 12,
    marginTop: 8,
    alignItems: 'center',
  },
  buttonText: {
    color: '#FFF',
    fontSize: 18,
    fontWeight: '700',
  },
  link: {
    marginTop: 16,
    alignItems: 'center',
  },
  linkText: {
    color: '#EEFFFF',
    fontSize: 14,
  },
});
