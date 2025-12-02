import React, { useEffect, useState } from "react";
import {
  View,
  Text,
  ScrollView,
  StyleSheet,
  ImageBackground,
  TouchableOpacity,
} from "react-native";
import { SafeAreaView, SafeAreaProvider } from "react-native-safe-area-context";
import AsyncStorage from "@react-native-async-storage/async-storage";
import * as Constants from "../../constants";
import GlobalStyles from "../../styles/GlobalStyleSheet";
import { FontAwesome } from '@expo/vector-icons';
import { useFocusEffect, router } from "expo-router";

// defining the Bundle type
type Bundle = {  
  id: number;  
  name: string;  
  created_at: number;  
  items: string[];  
};

export default function Profile() {
  // state variables for user profile and bundles
  const [username, setUsername] = useState<string>("");  
  const [email, setEmail] = useState<string>("");  
  const [bundleCount, setBundleCount] = useState(0);  
  const [avgRating, setAvgRating] = useState(0);  
  const [bundles, setBundles] = useState<Bundle[]>([]);  

  useFocusEffect(  
    // fetching profile data on focus
    React.useCallback(() => {
      let isActive = true;  
      (async () => {
        const token = await AsyncStorage.getItem("token");  
        if (!token) return;  
        try {
          const res = await fetch(`${Constants.IP_ADDRESS}/api/profile`, {  
            headers: { Authorization: `Bearer ${token}` },  
          });
          if (!res.ok) throw new Error(`Status ${res.status}`);  
          const data = await res.json();  
          if (isActive) {  
            setUsername(data.username);  
            setEmail(data.email);  
            setBundleCount(data.bundleCount);  
            setAvgRating(Number(data.avgRating));  
            setBundles(data.bundles);  
          }
        } catch (err) {
          console.error("Failed to load profile:", err);  
        }
      })();
      return () => { isActive = false };  
    }, [])  
  );

  // logout function
  const handleLogout = async () => {
    await AsyncStorage.removeItem("token");  
    router.replace("/login");  
  };

  return (
    <ImageBackground
      source={require("../../assets/images/dark_blue.jpg")}  
      style={styles.background}
    >
      <SafeAreaProvider>
        <SafeAreaView style={styles.safeArea} edges={["top", "bottom"]}>
          <ScrollView contentContainerStyle={styles.scrollContent}>
            <View style={styles.wrapper}>
              <Text style={[GlobalStyles.title, { textAlign: "center", marginTop: 50, marginBottom: 20 }]}>
                Profile
              </Text>

              <View style={styles.accountCard}>
                <View style={styles.accountHeaderRow}>
                  <FontAwesome name="user-circle" size={24} color="#002F6A" style={{ marginRight: 8 }} />
                  <Text style={styles.accountHeader}>Account</Text>
                </View>
                <View style={styles.accountInfoRow}>
                  <Text style={styles.accountLabel}>Username:</Text>
                  <Text style={styles.accountValue}>{username}</Text>
                </View>
                <View style={styles.accountInfoRow}>
                  <Text style={styles.accountLabel}>Email:</Text>
                  <Text style={styles.accountValue}>{email}</Text>
                </View>
              </View>

              <View style={styles.statsCard}>
                <View style={styles.statItem}>
                  <Text style={styles.statValue}>{bundleCount}</Text>
                  <Text style={styles.statLabel}>Bundles</Text>
                </View>
                <View style={styles.statItem}>
                  <Text style={styles.statValue}>{avgRating.toFixed(1)}</Text>
                  <Text style={styles.statLabel}>Avg Stars</Text>
                </View>
              </View>

              <TouchableOpacity
                style={styles.logoutButton}
                onPress={handleLogout}
              >
                <Text style={styles.logoutButtonText}>Log Out</Text>
              </TouchableOpacity>

              <Text style={[GlobalStyles.header, styles.historyTitle]}>
                Bundle History
              </Text>
              <View style={GlobalStyles.divider} />

              {bundles.map((b) => (
                <View key={b.id} style={styles.bundleCard}>
                  <View style={styles.bundleHeader}>
                    <Text style={styles.label}>{b.name}</Text>
                    <Text style={styles.timestamp}>
                      {new Date(b.created_at).toLocaleString(undefined, {
                        year: "numeric",
                        month: "long",
                        day: "numeric",
                        hour: "2-digit",
                        minute: "2-digit",
                      })}
                    </Text>
                  </View>
                  <View style={styles.foodItems}>
                    {b.items.map((item, i) => (
                      <Text key={i} style={styles.foodItem}>
                        • {item}
                      </Text>
                    ))}
                  </View>
                </View>
              ))}
            </View>
          </ScrollView>
        </SafeAreaView>
      </SafeAreaProvider>
    </ImageBackground>
  );
}

const styles = StyleSheet.create({
  safeArea: {
    flex: 1,
  },
  scrollContent: {
    flexGrow: 1,
    padding: 10,
  },
  wrapper: {
    flex: 1,
    paddingHorizontal: 6,
  },
accountCard: {
  backgroundColor: "#fff",
  borderRadius: 12,
  padding: 12,
  marginVertical: 12,
  shadowColor: "#000",
  shadowOffset: { width: 0, height: 2 },
  shadowOpacity: 0.08,
  shadowRadius: 4,
  elevation: 2,
},
accountHeaderRow: {
  flexDirection: "row",
  alignItems: "center",
  marginBottom: 8,
},
accountHeader: {
  fontSize: 18,
  fontWeight: "600",
  color: "#002F6A",
  },

accountInfo: {
  gap: 6,
},
accountInfoRow: {
  flexDirection: "row",
  alignItems: "center",
  marginTop: 6,
},
accountLabel: {
  fontSize: 14,
  fontWeight: "500",
  color: "#666",
  marginRight: 4,
},
accountValue: {
  fontSize: 15,
  fontWeight: "bold",
  color: "#333",
},
  accountText: {
    fontWeight: 'bold',
    fontSize: 16,
    color: "#333",
    marginBottom: 5,
  },
  background: {
    flex: 1,
    width: "100%",
    height: "100%",
  },
  statsCard: {
    backgroundColor: "#1a88db",
    borderColor: 'rgba(255, 255, 255, 0.08)',
    borderWidth: 3,
    borderRadius: 8,
    padding: 15,
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginTop:4,
    marginBottom: 25,
  },
  statItem: { alignItems: "center" },
  statValue: {
    fontSize: 24,
    fontWeight: "bold",
    color: "white"
  },
  statLabel: {
    fontSize: 14,
    color: "white",
    fontWeight: "bold",
    marginTop: 4
  },
  logoutButton: {
    backgroundColor: "#063E68",
    borderColor: 'rgba(255, 255, 255, 0.08)',
    borderWidth: 1,
    paddingVertical: 10,
    borderRadius: 8,
    alignItems: "center",
    marginBottom: 20,
  },
  logoutButtonText: {
    color: "white",
    fontSize: 16,
    fontWeight: "bold",
  },
  historyTitle: {
    fontWeight: "normal",
    marginTop: 10,
    marginBottom: 10,
  },
  bundleCard: {
    backgroundColor: "#F5F5F5",
    marginTop: 8,
    borderRadius: 8,
    padding: 15,
    marginBottom: -3,
  },
  bundleHeader: {
    flexDirection: "row",
    justifyContent: "space-between",
    marginBottom: 10,
  },
  timestamp: { fontSize: 12, color: "gray" },
  label: { fontSize: 16, fontWeight: "bold", color: "#333" },
  foodItems: { marginLeft: 8 },
  foodItem: { fontSize: 16, color: "#333", marginBottom: 4 },
});
