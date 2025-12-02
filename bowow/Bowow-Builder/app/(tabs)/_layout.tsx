import React from 'react';
import { Redirect, Stack, Tabs } from 'expo-router';
import FontAwesome from '@expo/vector-icons/Feather';
import { Text } from 'react-native';

// set the initial route for the app
export const unstable_settings = {
  initialRouteName: '(root)',
};

// define the layout component with tab navigation
const Layout = () => {
  return (
    // hide the header for all tabs
    <Tabs screenOptions={{ headerShown: false }}>
      {/* home tab */}
      <Tabs.Screen 
        name="index" 
        options={{ 
          title: 'Home', 
          tabBarIcon: ({ color }) => <FontAwesome size={28} name="home" color={color} />
        }} 
      />
      {/* shop tab */}
      <Tabs.Screen 
        name="shop" 
        options={{ 
          title: 'Shop',
          tabBarIcon: ({ color }) => <FontAwesome size={28} name="shopping-bag" color={color} />
        }} 
      />
      {/* cart tab */}
      <Tabs.Screen 
        name="cart" 
        options={{ 
          title: 'Cart',
          tabBarIcon: ({ color }) => <FontAwesome size={28} name="shopping-cart" color={color} />
        }} 
      />
      {/* profile tab */}
      <Tabs.Screen 
        name="profile" 
        options={{ 
          title: 'Profile',
          tabBarIcon: ({ color }) => <FontAwesome size={28} name="user" color={color} />
        }} 
      />
    </Tabs>
  );
};

export default Layout;
